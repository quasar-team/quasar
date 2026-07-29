#!/usr/bin/env python3
"""Tests the CMake generator-selection contract of generateCmake.py.

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>

Guards the OPCUA-3360 fix at the argv level: on Windows, with neither
QUASAR_CMAKE_GENERATOR nor QUASAR_CMAKE_GENERATOR_PLATFORM set, quasar must
pass NO -G flag so CMake auto-detects the newest installed Visual Studio.
That no-env path is exactly what a developer on a BE-ICS-provisioned box
gets out of the box, while CI's build jobs always export
QUASAR_CMAKE_GENERATOR=Ninja -- so without this test the default path has
no coverage at all. Also pins the env-override behaviour (-G, and -A only
alongside -G), the Linux --builder behaviour, and the absence of the dead
hardcoded "Visual Studio 15 2017 Win64" generator.

Hermetic: the quasar module graph around generateCmake is stubbed and the
cmake invocation is captured instead of executed -- no CMake, no compiler,
no OPC-UA backend, any host OS (platform.system() is forced per case).

Usage: python3 .CI/test_generator_selection.py
  (run from the quasar root directory)
"""

import importlib.util
import os
import platform
import sys
import tempfile
import types

QUASAR_ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
FRAMEWORK_INTERNALS = os.path.join(QUASAR_ROOT, 'FrameworkInternals')

DEAD_GENERATOR = "Visual Studio 15 2017"

captured = []


def make_stub(name, **attrs):
    module = types.ModuleType(name)
    for key, value in attrs.items():
        setattr(module, key, value)
    sys.modules[name] = module


class _TransformKeys:
    AS_CMAKE = "AS_CMAKE"
    D_BASE_CMAKE = "D_BASE_CMAKE"
    D_CMAKE = "D_CMAKE"


def _extract_argument(in_data, key):
    """Faithful re-implementation of quasar_basic_utils.extract_argument."""
    if key in in_data:
        pos = in_data.index(key)
        out = in_data
        out.pop(pos)
        value = out.pop(pos)
        return (out, value)
    return (in_data, None)


# Stub the module graph generateCmake imports, so importing it needs neither
# the codegen dependencies (jinja2, lxml) nor a quasar project. The one stub
# that matters is subprocessWithImprovedErrors: it captures the cmake argv
# instead of executing it.
make_stub("transformDesign",
          TransformKeys=_TransformKeys, transformByKey=lambda *a, **k: None)
make_stub("externalToolCheck",
          subprocessWithImprovedErrors=lambda argv, cmd: captured.append(list(argv)))
make_stub("commandMap", getCommand=lambda name: name)
make_stub("quasarExceptions", Mistake=type("Mistake", (Exception,), {}))
make_stub("manage_files", symlinkRuntimeDeps=lambda *a, **k: None)
make_stub("merge_design_and_meta",
          merge_user_and_meta_design=lambda user_file, meta_file, merged_file:
              (user_file.close(), meta_file.close()))
make_stub("quasar_basic_utils", extract_argument=_extract_argument)

spec = importlib.util.spec_from_file_location(
    "generateCmake", os.path.join(FRAMEWORK_INTERNALS, "generateCmake.py"))
generate_cmake = importlib.util.module_from_spec(spec)
spec.loader.exec_module(generate_cmake)


def run_case(forced_os, extra_args, env):
    """Run generateCmake under a forced OS and env; return the cmake argv."""
    captured.clear()
    for var in ("QUASAR_CMAKE_GENERATOR", "QUASAR_CMAKE_GENERATOR_PLATFORM"):
        os.environ.pop(var, None)
    os.environ.update(env)
    platform.system = lambda: forced_os
    context = {"projectSourceDir": QUASAR_ROOT,
               "projectBinaryDir": tempfile.mkdtemp(prefix="quasar_gsel_")}
    generate_cmake.generateCmake(context, *extra_args)
    assert len(captured) == 1, "expected exactly one cmake invocation"
    return captured[0]


def check(description, argv, expect_g=None, expect_a=None):
    """Assert on -G / -A presence+value; None means the flag must be absent."""
    problems = []
    for flag, expected in (("-G", expect_g), ("-A", expect_a)):
        value = argv[argv.index(flag) + 1] if flag in argv else None
        if expected is None and flag in argv:
            problems.append(f"unexpected {flag} {value}")
        elif expected is not None and value != expected:
            problems.append(f"wanted {flag} {expected}, got {value}")
    if any(DEAD_GENERATOR in token for token in argv):
        problems.append(f"dead hardcoded generator present: {DEAD_GENERATOR}")
    if "-DCMAKE_BUILD_TYPE=Release" not in argv:
        problems.append("missing -DCMAKE_BUILD_TYPE=Release")

    print(f"{description}\n  -> {' '.join(argv)}")
    if problems:
        for problem in problems:
            print(f"  FAILED: {problem}")
        return False
    print("  PASSED")
    return True


results = []

# The path BE-ICS developers get out of the box: nothing set -> no -G, CMake
# auto-detects the newest installed Visual Studio (the OPCUA-3360 fix).
results.append(check(
    "Windows, no env, no --builder (auto-detect default)",
    run_case("Windows", [], {})))

results.append(check(
    "Windows, QUASAR_CMAKE_GENERATOR=Ninja (the CI path)",
    run_case("Windows", [], {"QUASAR_CMAKE_GENERATOR": "Ninja"}),
    expect_g="Ninja"))

results.append(check(
    "Windows, generator + QUASAR_CMAKE_GENERATOR_PLATFORM=x64",
    run_case("Windows", [], {"QUASAR_CMAKE_GENERATOR": "Visual Studio 17 2022",
                             "QUASAR_CMAKE_GENERATOR_PLATFORM": "x64"}),
    expect_g="Visual Studio 17 2022", expect_a="x64"))

# -A is only meaningful next to -G: a platform var alone must change nothing.
results.append(check(
    "Windows, QUASAR_CMAKE_GENERATOR_PLATFORM alone (must stay auto-detect)",
    run_case("Windows", [], {"QUASAR_CMAKE_GENERATOR_PLATFORM": "x64"})))

results.append(check(
    "Linux, default builder",
    run_case("Linux", [], {})))

results.append(check(
    "Linux, --builder Ninja",
    run_case("Linux", ["--builder", "Ninja"], {}),
    expect_g="Ninja"))

if all(results):
    print(f"\nGENERATOR SELECTION TEST PASSED: all {len(results)} cases.")
    sys.exit(0)
print(f"\n{sum(1 for r in results if not r)} of {len(results)} cases FAILED.")
sys.exit(1)
