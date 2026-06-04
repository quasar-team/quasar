#!/usr/bin/env python3
# encoding: utf-8
'''
deviceClassAnalyzer.py

Diagnostic and migration tools for D<Class> device logic files.
Provides device_report (override status check) and addOverrideToDeviceClasses
(automated override keyword migration for upgrade_project).

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>

@copyright:  2026 CERN

@license:
Copyright (c) 2026, CERN.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import os
import re
from colorama import Fore, Style
from DesignInspector import DesignInspector
from transform_filters import cap_first


def _param_type_matches(expected_type, declared_params):
    """True if the C++ type `expected_type` appears as a whole token in declared_params.
    Whitespace-tolerant (astyle may pad std::vector< >); the lookbehind/lookahead
    boundaries keep OpcUa_Int16 from matching inside UInt16 and stop a scalar from
    matching inside std::vector<...> (shape drift)."""
    tokens = re.findall(r'\w+|[^\w\s]', expected_type)
    ws = r'\s*'   # separate var: a backslash cannot live in an f-string field (py<3.12)
    pattern = rf'(?<![\w<]){ws.join(re.escape(t) for t in tokens)}(?![\w<])'
    return bool(re.search(pattern, declared_params))


def _get_expected_delegate_param_types(designInspector, className):
    """Returns {method: [expected C++ param type, ...]} for every device-logic delegate
    whose param type carries the design dataType, so a dataType drift (the delegate
    compiles but no longer overrides Base_D -- silently unbound, OPCUA-3367) can be
    caught. Matches Device/templates/commonDeviceTemplates.jinja:
      delegated cache-var write<Cv> -- scalar '<dataType>', array 'std::vector<<dataType>>';
      source-var read<Sv> / write<Sv> -- always bare '<dataType>' (no array branch);
      method call<Name> -- one type per argument + return value (Oracle-mapped, but the
                           type token reduces to the same scalar/array dataType form)."""
    def _scalar_or_array(node):
        dataType = node.get('dataType')
        return f"std::vector<{dataType}>" if len(getattr(node, 'array', [])) > 0 else dataType
    types = {}
    for cv in designInspector.objectify_cache_variables(className, "[@addressSpaceWrite='delegated']"):
        types[f"write{cap_first(cv.get('name'))}"] = [_scalar_or_array(cv)]
    for sv in designInspector.objectify_source_variables(className):
        if sv.get('addressSpaceRead') in ('asynchronous', 'synchronous'):
            types[f"read{cap_first(sv.get('name'))}"] = [sv.get('dataType')]
        if sv.get('addressSpaceWrite') in ('asynchronous', 'synchronous'):
            types[f"write{cap_first(sv.get('name'))}"] = [sv.get('dataType')]
    for m in designInspector.objectify_methods(className):
        params = [_scalar_or_array(a) for a in getattr(m, 'argument', [])] + \
                 [_scalar_or_array(r) for r in getattr(m, 'returnvalue', [])]
        if params:
            types[f"call{cap_first(m.get('name'))}"] = params
    return types


def _get_expected_virtual_methods(designInspector, className):
    """Returns list of method name stems that Base_D<className> declares as virtual."""
    methods = []
    this = designInspector.objectify_class(className)
    for cv in designInspector.objectify_cache_variables(className, "[@addressSpaceWrite='delegated']"):
        methods.append(f"write{cap_first(cv.get('name'))}")
    for sv in designInspector.objectify_source_variables(className):
        if sv.get('addressSpaceRead') in ('asynchronous', 'synchronous'):
            methods.append(f"read{cap_first(sv.get('name'))}")
        if sv.get('addressSpaceWrite') in ('asynchronous', 'synchronous'):
            methods.append(f"write{cap_first(sv.get('name'))}")
    for m in getattr(this, 'method', []):
        methods.append(f"call{cap_first(m.get('name'))}")
    return methods


def deviceReport(context):
    """Reports which Base_D virtual methods are overridden in D<Class> and whether override keyword is present."""
    srcDir = context['projectSourceDir']
    designInspector = DesignInspector(os.path.join(srcDir, 'Design', 'Design.xml'))
    classes = designInspector.get_names_of_all_classes(only_with_device_logic=True)
    has_issues = False

    for className in classes:
        header_path = os.path.join(srcDir, 'Device', 'include', f'D{className}.h')
        cpp_path = os.path.join(srcDir, 'Device', 'src', f'D{className}.cpp')
        expected = _get_expected_virtual_methods(designInspector, className)
        expected_delegate_types = _get_expected_delegate_param_types(designInspector, className)

        if not expected:
            print(f"  D{className}: (no virtual device methods)")
            continue

        # Read header if it exists
        header_content = ''
        if os.path.isfile(header_path):
            with open(header_path, 'r') as f:
                header_content = f.read()
        else:
            print(f"  D{className}: {Fore.YELLOW}header not found{Style.RESET_ALL}")
            continue

        # Read cpp if it exists
        cpp_content = ''
        if os.path.isfile(cpp_path):
            with open(cpp_path, 'r') as f:
                cpp_content = f.read()

        for method_name in expected:
            # Check if method is declared in header (anchored to UaStatus to avoid substring matches)
            decl_pattern = rf'UaStatus\s+{re.escape(method_name)}\s*\(([^)]*)\)'
            decl_match = re.search(decl_pattern, header_content, re.DOTALL)
            declared = bool(decl_match)
            # Check for override keyword in the declaration
            override_pattern = rf'UaStatus\s+{re.escape(method_name)}\s*\([^)]*\)\s*override\s*;'
            has_override = bool(re.search(override_pattern, header_content, re.DOTALL)) if declared else False
            # Check if method is implemented in cpp
            impl_pattern = rf'D{re.escape(className)}::{re.escape(method_name)}\s*\('
            implemented = bool(re.search(impl_pattern, cpp_content))
            # Check if implementation is just the default stub
            is_stub = False
            if implemented:
                stub_pattern = rf'D{re.escape(className)}::{re.escape(method_name)}\s*\([^)]*\)\s*\{{[^}}]*OpcUa_BadNotImplemented[^}}]*\}}'
                is_stub = bool(re.search(stub_pattern, cpp_content, re.DOTALL))

            # Each delegate's declared param type(s) must still match the design; a drifted
            # delegate compiles but no longer overrides Base_D, so it is never bound (OPCUA-3367).
            type_mismatch = False
            missing_types = []
            if declared and method_name in expected_delegate_types:
                missing_types = [t for t in expected_delegate_types[method_name]
                                 if not _param_type_matches(t, decl_match.group(1))]
                type_mismatch = bool(missing_types)

            if not declared and not implemented:
                print(f"  D{className}::{method_name}: {Fore.GREEN}using Base_D default{Style.RESET_ALL}")
            elif declared and not has_override:
                print(f"  D{className}::{method_name}: {Fore.RED}MISSING override keyword{Style.RESET_ALL}")
                has_issues = True
            elif type_mismatch:
                print(f"  D{className}::{method_name}: {Fore.RED}delegate type mismatch (expected '{', '.join(missing_types)}', design drifted -- delegate silently unbound){Style.RESET_ALL}")
                has_issues = True
            elif is_stub:
                print(f"  D{className}::{method_name}: {Fore.YELLOW}declared but still returns BadNotImplemented{Style.RESET_ALL}")
            else:
                print(f"  D{className}::{method_name}: {Fore.GREEN}overridden{Style.RESET_ALL}")

    if has_issues:
        print(f"\n{Fore.YELLOW}Hint: run './quasar.py upgrade_project .' to add missing 'override' keywords automatically.{Style.RESET_ALL}")


def addOverrideToDeviceClasses(context):
    """Adds 'override' keyword to D<Class> method declarations that override Base_D virtuals."""
    srcDir = context['projectSourceDir']
    designInspector = DesignInspector(os.path.join(srcDir, 'Design', 'Design.xml'))
    classes = designInspector.get_names_of_all_classes(only_with_device_logic=True)
    total_fixed = 0

    for className in classes:
        header_path = os.path.join(srcDir, 'Device', 'include', f'D{className}.h')
        expected = _get_expected_virtual_methods(designInspector, className)

        if not expected or not os.path.isfile(header_path):
            continue

        with open(header_path, 'r') as f:
            content = f.read()

        original = content
        fixed_count = 0
        for method_name in expected:
            # Match: UaStatus methodName ( ... ) ; (without override)
            # re.DOTALL handles multi-line signatures
            pattern = rf'(UaStatus\s+{re.escape(method_name)}\s*\([^)]*\))\s*;'
            def add_override(match):
                full = match.group(0)
                if 'override' in full:
                    return full
                nonlocal fixed_count
                fixed_count += 1
                return match.group(1) + ' override;'
            content = re.sub(pattern, add_override, content, flags=re.DOTALL)

        if content != original:
            with open(header_path, 'w') as f:
                f.write(content)
            print(f"  D{className}.h: added 'override' to {fixed_count} method(s)")
            total_fixed += fixed_count
        else:
            print(f"  D{className}.h: no changes needed")

    if total_fixed > 0:
        print(f"\n{Fore.GREEN}Added 'override' to {total_fixed} method declaration(s) total.{Style.RESET_ALL}")
    else:
        print(f"\n{Fore.GREEN}All device classes already have proper 'override' keywords.{Style.RESET_ALL}")
