#!/usr/bin/env python3
"""Tests the upgrade migration path: old-style D<Class> files (no override keyword)
get the 'override' keyword added by addOverrideToDeviceClasses.

This test uses the test_cache_variables Design.xml which has many delegated cache variables,
ensuring the regex-based override addition handles real-world patterns.

Usage: python3 .CI/test_upgrade_override.py
  (run from the quasar root directory)
"""

import os
import sys
import tempfile
import shutil

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'FrameworkInternals'))

from deviceClassAnalyzer import addOverrideToDeviceClasses

QUASAR_ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')

# Old-style D header: method declarations WITHOUT override keyword
OLD_STYLE_HEADER = """\
#ifndef __DTestClass__H__
#define __DTestClass__H__

#include <Base_DTestClass.h>

namespace Device
{

class DTestClass : public Base_DTestClass
{
public:
  explicit DTestClass (
    const Configuration::TestClass& config,
    Parent_DTestClass* parent
  );
  ~DTestClass ();

  /* delegators for cachevariables and sourcevariables */
  UaStatus writeMyDelegatedVar ( const OpcUa_Double& v);
  UaStatus writeMyDelegatedArray ( const std::vector<OpcUa_Int32>& v);

  /* multi-line source variable signature */
  UaStatus readMySourceVar (
    OpcUa_Double& value,
    UaDateTime& sourceTime
  );

  UaStatus writeMySourceVar (
    OpcUa_Double& value
  );

  /* method */
  UaStatus callMyMethod (
    const OpcUa_Double& arg1,
    OpcUa_Double& result
  );

  // ----------------------------------------------------------------------- *
  // -     CUSTOM CODE STARTS BELOW THIS COMMENT.                            *
  // -     Don't change this comment, otherwise merge tool may be troubled.  *
  // ----------------------------------------------------------------------- *

public:
  void myCustomMethod();

private:
  DTestClass( const DTestClass& other );
  DTestClass& operator=(const DTestClass& other);

  double m_customMember;
};

}
#endif
"""

# Minimal Design.xml that declares the same methods
MINIMAL_DESIGN = """\
<?xml version="1.0" encoding="UTF-8"?>
<d:design xmlns:d="http://cern.ch/quasar/Design" projectShortName="test">
  <d:class name="TestClass">
    <d:devicelogic/>
    <d:cachevariable name="myDelegatedVar" dataType="OpcUa_Double" addressSpaceWrite="delegated" initializeWith="valueAndStatus" initialStatus="OpcUa_BadWaitingForInitialData" initialValue="0" nullPolicy="nullForbidden"/>
    <d:cachevariable name="myDelegatedArray" dataType="OpcUa_Int32" addressSpaceWrite="delegated" initializeWith="valueAndStatus" initialStatus="OpcUa_BadWaitingForInitialData" nullPolicy="nullForbidden">
      <d:array minimumSize="1" maximumSize="10"/>
    </d:cachevariable>
    <d:sourcevariable name="mySourceVar" dataType="OpcUa_Double" addressSpaceRead="synchronous" addressSpaceWrite="synchronous"/>
    <d:method name="myMethod">
      <d:argument name="arg1" dataType="OpcUa_Double"/>
      <d:returnvalue name="result" dataType="OpcUa_Double"/>
    </d:method>
  </d:class>
  <d:root>
    <d:hasobjects instantiateUsing="configuration" class="TestClass"/>
  </d:root>
</d:design>
"""


def test_upgrade_adds_override():
    """Create a temp project with old-style D files, run upgrade, verify override was added."""
    tmpdir = tempfile.mkdtemp(prefix='quasar_upgrade_test_')
    try:
        # Set up minimal project structure
        os.makedirs(os.path.join(tmpdir, 'Design'))
        os.makedirs(os.path.join(tmpdir, 'Device', 'include'))
        os.makedirs(os.path.join(tmpdir, 'Device', 'src'))

        # Write Design.xml
        with open(os.path.join(tmpdir, 'Design', 'Design.xml'), 'w') as f:
            f.write(MINIMAL_DESIGN)

        # Write old-style header (no override keywords)
        header_path = os.path.join(tmpdir, 'Device', 'include', 'DTestClass.h')
        with open(header_path, 'w') as f:
            f.write(OLD_STYLE_HEADER)

        # Verify: no 'override' in original
        with open(header_path, 'r') as f:
            original = f.read()
        assert 'override' not in original, "Test setup error: original should not contain 'override'"

        # Run the upgrade
        context = {'projectSourceDir': tmpdir}
        addOverrideToDeviceClasses(context)

        # Read result
        with open(header_path, 'r') as f:
            upgraded = f.read()

        # Verify all 5 methods got override added
        expected_overrides = [
            'writeMyDelegatedVar',
            'writeMyDelegatedArray',
            'readMySourceVar',
            'writeMySourceVar',
            'callMyMethod',
        ]

        errors = []
        for method in expected_overrides:
            if f'{method}' not in upgraded:
                errors.append(f"Method {method} disappeared from file!")
                continue
            # Check that override appears between method name and semicolon
            # Find the line(s) containing the method and check for override
            if f'override' not in upgraded.split(method)[1].split(';')[0]:
                errors.append(f"Method {method} is missing 'override' keyword after upgrade")

        # Verify custom code was preserved
        if 'myCustomMethod' not in upgraded:
            errors.append("Custom method 'myCustomMethod' was lost during upgrade!")
        if 'm_customMember' not in upgraded:
            errors.append("Custom member 'm_customMember' was lost during upgrade!")

        if errors:
            print(f"\n{'='*60}")
            print("UPGRADE TEST FAILED:")
            for e in errors:
                print(f"  - {e}")
            print(f"{'='*60}")
            print(f"\nUpgraded file content:\n{upgraded}")
            return False
        else:
            print("UPGRADE TEST PASSED: all 5 methods got 'override', custom code preserved.")
            return True

    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)


def test_idempotent():
    """Run upgrade twice -- second run should make no changes."""
    tmpdir = tempfile.mkdtemp(prefix='quasar_upgrade_idem_')
    try:
        os.makedirs(os.path.join(tmpdir, 'Design'))
        os.makedirs(os.path.join(tmpdir, 'Device', 'include'))

        with open(os.path.join(tmpdir, 'Design', 'Design.xml'), 'w') as f:
            f.write(MINIMAL_DESIGN)

        header_path = os.path.join(tmpdir, 'Device', 'include', 'DTestClass.h')
        with open(header_path, 'w') as f:
            f.write(OLD_STYLE_HEADER)

        context = {'projectSourceDir': tmpdir}

        # First run
        addOverrideToDeviceClasses(context)
        with open(header_path, 'r') as f:
            after_first = f.read()

        # Second run (should be no-op)
        addOverrideToDeviceClasses(context)
        with open(header_path, 'r') as f:
            after_second = f.read()

        if after_first == after_second:
            print("IDEMPOTENCY TEST PASSED: second run made no changes.")
            return True
        else:
            print("IDEMPOTENCY TEST FAILED: second run modified the file!")
            return False
    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)


if __name__ == '__main__':
    results = []
    results.append(test_upgrade_adds_override())
    results.append(test_idempotent())

    if all(results):
        print(f"\nAll {len(results)} tests passed.")
        sys.exit(0)
    else:
        print(f"\n{sum(1 for r in results if not r)} of {len(results)} tests FAILED.")
        sys.exit(1)
