#!/usr/bin/env python3
"""device_report must catch a delegated cache-variable whose D<Class> delegate's
parameter type no longer matches the design (OPCUA-3367).

When a delegated cache variable's dataType changes, the frozen (skip-if-exists)
D<Class> delegate keeps the OLD type. In the standard path the `override` keyword
turns that into a compile error -- but a legacy / hand-written delegate without
`override` (or one a user de-`override`s to silence the red build) drifts
SILENTLY: it compiles, the server runs, the delegate binds to the old type, and
the only diagnostic -- device_report -- is blind, because it matches delegates by
method NAME, never parameter type. So a type-drifted (silently unbound) delegate
gets the same clean verdict as a correct one.

Captures Piotr Nikiel's v2.0.0-review insight: "changed the delegated data type
and my delegate is never called and it all builds and runs."

The test gives device_report a CORRECT delegate and a TYPE-DRIFTED one -- same
method, same D<Class>, only the design's dataType differs -- and asserts the two
get DIFFERENT verdicts (i.e. device_report actually flags the drift). On today's
master both get the identical "overridden" verdict (name-only matching), so the
test is RED; a type-aware device_report makes it GREEN.

Behavioural test in the .CI/test_upgrade_override.py family. Hermetic: no
compiler and no OPC-UA backend; it drives the real deviceReport.

Usage: python3 .CI/test_no_unbound_delegate.py   (run from the quasar root)
"""

import os
import sys
import io
import re
import tempfile
import shutil
import contextlib

QUASAR_ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.insert(0, os.path.join(QUASAR_ROOT, 'FrameworkInternals'))

from deviceClassAnalyzer import deviceReport

_ANSI = re.compile(r'\x1b\[[0-9;]*m')

# Only the DESIGN's dataType varies between the two cases; the user's frozen
# delegate is fixed at OpcUa_Double. Design==Double is correct; Design==Float is
# the type drift (delegate still Double).
_DESIGN = '''<?xml version="1.0" encoding="UTF-8"?>
<d:design xmlns:d="http://cern.ch/quasar/Design" projectShortName="repro">
  <d:class name="Sensor">
    <d:devicelogic/>
    <d:cachevariable name="reading" dataType="{dtype}" addressSpaceWrite="delegated"
       initializeWith="valueAndStatus" initialStatus="OpcUa_BadWaitingForInitialData"
       initialValue="0" nullPolicy="nullForbidden"/>
    <d:sourcevariable name="sample" dataType="{dtype}"
       addressSpaceRead="synchronous" addressSpaceWrite="synchronous"
       addressSpaceReadUseMutex="no" addressSpaceWriteUseMutex="no"/>
    <d:method name="doThing" executionSynchronicity="synchronous" addressSpaceCallUseMutex="no">
      <d:argument name="x" dataType="{dtype}"/>
    </d:method>
  </d:class>
  <d:root><d:hasobjects instantiateUsing="configuration" class="Sensor"/></d:root>
</d:design>'''

_DSENSOR_H = '''#ifndef DSensor_H_
#define DSensor_H_
#include <Base_DSensor.h>
namespace Device {
class DSensor : public Base_DSensor {
public:
  UaStatus writeReading ( const OpcUa_Double& v) override;
  UaStatus readSample ( OpcUa_Double& value, UaDateTime& sourceTime ) override;
  UaStatus writeSample ( OpcUa_Double& value ) override;
  UaStatus callDoThing ( OpcUa_Double x ) override;
};
}
#endif'''

_DSENSOR_CPP = '''#include <DSensor.h>
namespace Device {
UaStatus DSensor::writeReading ( const OpcUa_Double& v) { return OpcUa_Good; }
UaStatus DSensor::readSample ( OpcUa_Double& value, UaDateTime& sourceTime ) { return OpcUa_Good; }
UaStatus DSensor::writeSample ( OpcUa_Double& value ) { return OpcUa_Good; }
UaStatus DSensor::callDoThing ( OpcUa_Double x ) { return OpcUa_Good; }
}'''


def _device_report_verdict(design_dtype, method):
    """Return device_report's verdict line (ANSI-stripped) for `method` in a project
    whose design dataType is `design_dtype` but whose delegate is fixed at Double."""
    tmp = tempfile.mkdtemp(prefix='quasar_unbound_')
    try:
        for d in ('Design', 'Device/include', 'Device/src'):
            os.makedirs(os.path.join(tmp, d))
        open(os.path.join(tmp, 'Design/Design.xml'), 'w').write(_DESIGN.format(dtype=design_dtype))
        open(os.path.join(tmp, 'Device/include/DSensor.h'), 'w').write(_DSENSOR_H)
        open(os.path.join(tmp, 'Device/src/DSensor.cpp'), 'w').write(_DSENSOR_CPP)
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            deviceReport({'projectSourceDir': tmp})
        lines = [_ANSI.sub('', l).strip() for l in buf.getvalue().splitlines()
                 if f'::{method}:' in l]
        return lines[0] if lines else f'({method} not reported)'
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


def test_device_report_catches_delegate_type_drift():
    # Every device-logic delegate whose generated param type carries the design dataType:
    # delegated cache-var write, source-var read + write, and method call. A dataType drift
    # (delegate frozen at Double vs design Float) must yield a DIFFERENT verdict for each.
    # The method-call row is RED until device_report also type-checks methods (OPCUA-3369).
    ok = True
    for method, kind in (('writeReading', 'delegated cache-var write'),
                         ('readSample', 'source-var read'),
                         ('writeSample', 'source-var write'),
                         ('callDoThing', 'method call')):
        correct = _device_report_verdict('OpcUa_Double', method)  # design==delegate
        drifted = _device_report_verdict('OpcUa_Float', method)   # design!=delegate (DRIFT)
        print(f"  {kind} ({method}):")
        print(f"    correct (design Double): {correct!r}")
        print(f"    drifted (design Float):  {drifted!r}")
        if correct == drifted:
            ok = False
            print(f"    {'-'*70}")
            print(f"    FAILED: device_report gives the TYPE-DRIFTED {kind} delegate the SAME")
            print(f"            verdict as a correct one -- name-only matching, blind to the")
            print(f"            silent unbind (OPCUA-3367).")
    if ok:
        print("PASSED: device_report flags every type-drifted device-logic delegate.")
    return ok


if __name__ == '__main__':
    results = [test_device_report_catches_delegate_type_drift()]
    if all(results):
        print(f"\nAll {len(results)} tests passed.")
        sys.exit(0)
    else:
        print(f"\n{sum(1 for r in results if not r)} of {len(results)} tests FAILED.")
        sys.exit(1)
