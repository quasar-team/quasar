#!/usr/bin/env python3
"""quasar code generation must be a STABLE function of Design.xml: regenerating
with no change to Design.xml must NOT rewrite the outputs on disk -- otherwise
every output gets a fresh mtime on byte-identical content and CMake/make rebuild
everything.

Captures the regression Piotr Nikiel reported in the v2.0.0 review,
"the build is redone even when there is no change of Design" (OPCUA-3366).
There are exactly two unconditional-write root causes, and this file has one
test per cause:
  1. merge_design_and_meta.py  -> write_to_file          (DesignWithMeta.xml)
  2. transformDesign.py        -> transformDesignByJinja  (open('wb') + write)

Every design-derived Jinja generator funnels through that single
transformDesignByJinja write path, so ONE representative key proves the defect
for all of them -- we deliberately don't enumerate the rest. The representative
is a C++ (astyle) transform on purpose: the fix must skip the rewrite AFTER
indentation, not before.

Both tests also assert the two passes produce byte-IDENTICAL content (before the
mtime check), so plain non-deterministic codegen is caught too -- it just shows
up as "CONTENT CHANGED between identical runs" instead of a stale rebuild.

Behavioural/idempotency test in the same family as .CI/test_upgrade_override.py.
Hermetic: no compiler and no OPC-UA backend. It drives the real codegen entry
points -- the merge function and transformByKey, exactly what generateCmake and
the `generate` commands call.

Usage: python3 .CI/test_no_spurious_rebuild.py   (run from the quasar root)
"""

import os
import sys
import io
import tempfile
import shutil
import contextlib

QUASAR_ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.insert(0, os.path.join(QUASAR_ROOT, 'FrameworkInternals'))

from merge_design_and_meta import merge_user_and_meta_design
from transformDesign import TransformKeys, transformByKey, getTransformOutput

DESIGN = os.path.join(QUASAR_ROOT, 'Design', 'Design.xml')
META = os.path.join(QUASAR_ROOT, 'Meta', 'design', 'meta-design.xml')
OLD_MTIME = 1_000_000_000   # a fixed, clearly-old timestamp used to detect rewrites

# One representative Jinja generator. Every design-derived key flows through the
# same transformDesignByJinja write path, so a second key would re-test the
# identical line. AS_CLASS_CPP_ALL renders every class body AND is a C++ (astyle)
# transform, so it also exercises the post-indentation write path.
REPRESENTATIVE_KEY = TransformKeys.AS_CLASS_CPP_ALL


@contextlib.contextmanager
def _quiet():
    """Silence the generators' colorama progress prints."""
    with contextlib.redirect_stdout(io.StringIO()):
        yield


def _run_merge(binary_dir):
    """Root cause #1: design + meta merge -> DesignWithMeta.xml. Returns its path.

    Hands merge_user_and_meta_design the user/meta inputs and the DESTINATION
    PATH -- so the merge owns its output and can compare-before-writing. That is
    the only convention under which OPCUA-3366 is fixable: write-if-different
    must compare BEFORE the destination is truncated, which a caller-pre-opened
    'w' handle has already lost. This is also how generateCmake invokes it.
    Note for whoever fixes OPCUA-3366: a merge that still writes unconditionally
    (or that cannot accept a destination path at all) fails this test -- by
    rewriting / by raising -- both reported as FAILED [merge]. DesignWithMeta.xml
    is `DEPENDS ${DESIGN_FILE}` of nearly every add_custom_command in the CMake
    tree, so rewriting it re-fires the whole build."""
    merged = os.path.join(binary_dir, 'Design', 'DesignWithMeta.xml')
    os.makedirs(os.path.dirname(merged), exist_ok=True)
    with _quiet(), open(DESIGN, 'r', encoding='utf-8') as user_f, \
            open(META, 'r', encoding='utf-8') as meta_f:
        merge_user_and_meta_design(user_f, meta_f, merged)
    return merged


def _run_jinja(binary_dir):
    """Root cause #2: a Jinja generator -> generated C++. Returns its output path.
    transformByKey needs DesignWithMeta.xml present, so run the merge first; it
    also resolves the template path relative to cwd, so callers must be at
    QUASAR_ROOT (as production codegen is)."""
    _run_merge(binary_dir)
    sd = {'context': {'projectSourceDir': QUASAR_ROOT, 'projectBinaryDir': binary_dir}}
    with _quiet():
        transformByKey(REPRESENTATIVE_KEY, sd)
    return getTransformOutput(REPRESENTATIVE_KEY, sd)


def _assert_not_rewritten(label, generate):
    """Generate once; backdate the output's mtime; generate again with the SAME
    input. The output must be byte-identical AND keep the old mtime (i.e. the
    generator skipped the write). Returns True on pass.

    A generator that does not yet support the write-if-different calling
    convention may raise instead of returning -- that is still a fail for this
    invariant, so we catch it and report it rather than letting it abort the
    whole suite (the other test must still run and report)."""
    binary_dir = tempfile.mkdtemp(prefix='quasar_norebuild_')
    try:
        try:
            path = generate(binary_dir)
            with open(path, 'rb') as f:
                before = f.read()
            os.utime(path, (OLD_MTIME, OLD_MTIME))

            generate(binary_dir)                  # identical input, second pass

            with open(path, 'rb') as f:
                after = f.read()
        except Exception as e:
            print(f"FAILED [{label}]: generator raised {type(e).__name__}: {e} "
                  "(does not support write-if-different regeneration; OPCUA-3366).")
            return False
        rel = os.path.relpath(path, binary_dir)

        if after != before:
            print(f"FAILED [{label}]: {rel} CONTENT CHANGED between identical runs "
                  "(non-deterministic codegen).")
            return False
        if int(os.stat(path).st_mtime) != OLD_MTIME:
            print(f"FAILED [{label}]: {rel} REWRITTEN with identical content "
                  "(generator is not write-if-different) "
                  "=> CMake/make rebuild on an unchanged Design (OPCUA-3366).")
            return False
        print(f"PASSED [{label}]: {rel} unchanged and not rewritten on identical re-generation.")
        return True
    finally:
        shutil.rmtree(binary_dir, ignore_errors=True)


def test_merge_is_write_if_different():
    return _assert_not_rewritten('merge', _run_merge)


def test_jinja_generator_is_write_if_different():
    return _assert_not_rewritten('jinja', _run_jinja)


if __name__ == '__main__':
    os.chdir(QUASAR_ROOT)   # transformByKey resolves templates relative to cwd
    results = [
        test_merge_is_write_if_different(),
        test_jinja_generator_is_write_if_different(),
    ]
    if all(results):
        print(f"\nAll {len(results)} tests passed.")
        sys.exit(0)
    else:
        print(f"\n{sum(1 for r in results if not r)} of {len(results)} tests FAILED.")
        sys.exit(1)
