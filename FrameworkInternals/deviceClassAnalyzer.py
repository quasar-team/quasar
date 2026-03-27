#!/usr/bin/env python3
# encoding: utf-8
'''
deviceClassAnalyzer.py

Diagnostic and migration tools for D<Class> device logic files.
Provides device_report (override status check) and addOverrideToDeviceClasses
(automated override keyword migration for upgrade_project).

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>

@copyright:  2025 CERN

@license:
Copyright (c) 2025, CERN.
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
            decl_pattern = rf'UaStatus\s+{re.escape(method_name)}\s*\('
            declared = bool(re.search(decl_pattern, header_content))
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

            if not declared and not implemented:
                print(f"  D{className}::{method_name}: {Fore.GREEN}using Base_D default{Style.RESET_ALL}")
            elif declared and not has_override:
                print(f"  D{className}::{method_name}: {Fore.RED}MISSING override keyword{Style.RESET_ALL}")
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
