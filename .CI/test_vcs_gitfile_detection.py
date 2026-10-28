#!/usr/bin/env python3
"""Framework invariant: VersionControlInterface recognises every git checkout
layout, not only the one where .git is a directory.

In a git submodule or a git worktree the checkout carries a .git FILE that
points at the real git directory (gitdir: ...). quasar used to probe .git with
os.path.isdir(), so check_consistency, style_it, enable_module/disable_module
and the commit ID stamped by meta_build_info.py failed in such checkouts with
"Project is unversioned or it was impossible to determine the version control
system used" (OPCUA-3488).

The test builds throwaway repositories in a temporary directory and drives the
real class against three layouts (plain clone, worktree, submodule) plus one
negative control (a directory with no .git at all, which must still be refused).
It needs git on PATH and pygit2, both of which quasar itself requires.

Usage: python3 .CI/test_vcs_gitfile_detection.py
  (run from the quasar root directory)
"""

import os
import shutil
import stat
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'FrameworkInternals'))

import version_control_interface  # noqa: E402

TRACKED_FILE = 'tracked.txt'
GIT_IDENTITY = ['-c', 'user.name=quasar-ci', '-c', 'user.email=quasar-ci@example.invalid']


def git(*args, cwd=None):
    cmd = ['git'] + GIT_IDENTITY + list(args)
    result = subprocess.run(cmd, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            universal_newlines=True)
    if result.returncode != 0:
        raise RuntimeError('%s failed (%d):\n%s' % (' '.join(cmd), result.returncode, result.stdout))
    return result.stdout


def make_base_repo(root):
    """A plain repository with one committed file. Its .git is a directory."""
    base = os.path.join(root, 'base')
    os.mkdir(base)
    git('init', '-q', '-b', 'main', cwd=base)
    with open(os.path.join(base, TRACKED_FILE), 'w') as f:
        f.write('tracked by git\n')
    git('add', TRACKED_FILE, cwd=base)
    git('commit', '-q', '-m', 'initial', cwd=base)
    return base


def make_worktree(base, root):
    """A linked worktree of the base repository. Its .git is a file."""
    wt = os.path.join(root, 'worktree')
    git('worktree', 'add', '-q', wt, cwd=base)
    return wt


def make_submodule(base, root):
    """The base repository added as a submodule of a fresh parent. Its .git is a file."""
    parent = os.path.join(root, 'parent')
    os.mkdir(parent)
    git('init', '-q', '-b', 'main', cwd=parent)
    # git >= 2.38.1 refuses file:// submodule sources unless told otherwise.
    git('-c', 'protocol.file.allow=always', 'submodule', 'add', '-q', base, 'server', cwd=parent)
    return os.path.join(parent, 'server')


def check_layout(label, path, expect_git_is_file):
    failures = []
    dot_git = os.path.join(path, '.git')
    if os.path.isfile(dot_git) != expect_git_is_file:
        failures.append('%s: expected .git to be a %s, found %s' % (
            label, 'file' if expect_git_is_file else 'directory',
            'file' if os.path.isfile(dot_git) else 'directory'))
    try:
        vci = version_control_interface.VersionControlInterface(path)
    except Exception as e:
        failures.append('%s: VersionControlInterface refused the checkout: %s' % (label, e))
        return failures
    if vci.vcs_type != 'git':
        failures.append('%s: vcs_type is %r, expected git' % (label, vci.vcs_type))
    if not vci.is_versioned(TRACKED_FILE):
        failures.append('%s: %s not reported as versioned' % (label, TRACKED_FILE))
    if vci.is_versioned('never-added.txt'):
        failures.append('%s: an untracked name reported as versioned' % label)
    commit = vci.get_latest_repo_commit()
    if 'Exception' in commit or 'Failed' in commit:
        failures.append('%s: get_latest_repo_commit did not resolve: %s' % (label, commit))
    return failures


def check_unversioned_is_refused(root):
    plain = os.path.join(root, 'plain')
    os.mkdir(plain)
    try:
        version_control_interface.VersionControlInterface(plain)
    except Exception:
        return []
    return ['unversioned directory: VersionControlInterface accepted a directory with no .git']


def rmtree(path):
    def make_writable(func, p, _exc):
        os.chmod(p, stat.S_IWRITE)
        func(p)
    shutil.rmtree(path, onerror=make_writable)


QUASAR_ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))


def run_quasar_tool(args, cwd):
    """Runs a quasar.py command or framework script with the interpreter running this test."""
    result = subprocess.run([sys.executable] + args, cwd=cwd, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, universal_newlines=True)
    return result.returncode, result.stdout


def check_commands_in_worktree(root):
    """The user's view: the framework's own commands inside a worktree of this checkout.

    The class checks above prove the detection; this proves what people run. A
    worktree of the quasar checkout is a gitfile layout of a real framework tree,
    so check_consistency must get past the detection and meta_build_info.py must
    stamp a real commit ID instead of the placeholder it writes when detection
    fails (that failure is caught, so without this check it would be silent).
    """
    failures = []
    wt = os.path.join(root, 'quasar-worktree')
    gen = os.path.join(root, 'generated')
    os.mkdir(gen)
    try:
        git('worktree', 'add', '-q', '--detach', wt, 'HEAD', cwd=QUASAR_ROOT)
    except RuntimeError as e:
        return ['worktree of the quasar checkout: could not be created: %s' % e]
    try:
        if not os.path.isfile(os.path.join(wt, '.git')):
            failures.append('worktree of the quasar checkout: .git is not a file, the layout under test is missing')
        # A worktree checks out HEAD. Overlay the working tree's framework scripts so an
        # uncommitted change to the detection is what gets exercised, not the last commit.
        shutil.copytree(os.path.join(QUASAR_ROOT, 'FrameworkInternals'), os.path.join(wt, 'FrameworkInternals'),
                        dirs_exist_ok=True, ignore=shutil.ignore_patterns('__pycache__', 'quasar-modules'))
        rc, out = run_quasar_tool(['quasar.py', 'check_consistency'], cwd=wt)
        if 'Determined vcs type: git' not in out or 'impossible to determine the version control system' in out:
            failures.append('check_consistency in a worktree did not detect git:\n%s' % out[-1500:])
        elif rc != 0:
            failures.append('check_consistency in a worktree exited %d:\n%s' % (rc, out[-1500:]))
        rc, out = run_quasar_tool(['FrameworkInternals/meta_build_info.py', '--project_root_dir', wt,
                                   '--target_generation_dir', gen, '--toolkit_libs', 'invariant-test'], cwd=wt)
        if rc != 0:
            failures.append('meta_build_info.py in a worktree exited %d:\n%s' % (rc, out[-1500:]))
        else:
            generated = ''
            for name in os.listdir(gen):
                with open(os.path.join(gen, name)) as f:
                    generated += f.read()
            commit_lines = [l for l in generated.splitlines() if 'GEND_COMMIT_ID' in l]
            if not commit_lines:
                failures.append('meta_build_info.py in a worktree wrote no GEND_COMMIT_ID')
            elif 'VCS type [git] commit ID [' not in commit_lines[0] or 'No commit ID found' in commit_lines[0] \
                    or 'Exception' in commit_lines[0]:
                failures.append('meta_build_info.py in a worktree did not resolve the commit: %s' % commit_lines[0])
    finally:
        try:
            git('worktree', 'remove', '--force', wt, cwd=QUASAR_ROOT)
        except RuntimeError:
            git('worktree', 'prune', cwd=QUASAR_ROOT)
    return failures


def main():
    root = tempfile.mkdtemp(prefix='quasar-vcs-gitfile-')
    failures = []
    try:
        base = make_base_repo(root)
        worktree = make_worktree(base, root)
        submodule = make_submodule(base, root)
        failures += check_layout('plain clone', base, expect_git_is_file=False)
        failures += check_layout('worktree', worktree, expect_git_is_file=True)
        failures += check_layout('submodule', submodule, expect_git_is_file=True)
        failures += check_unversioned_is_refused(root)
        failures += check_commands_in_worktree(root)
    finally:
        rmtree(root)

    for f in failures:
        print('FAIL: ' + f)
    if failures:
        print('%d failure(s): VersionControlInterface does not handle every git checkout layout (OPCUA-3488)' % len(failures))
        return 1
    print('OK: VersionControlInterface accepts plain, worktree and submodule checkouts and refuses an unversioned directory; '
          'check_consistency and meta_build_info.py work inside a worktree of this checkout')
    return 0


if __name__ == '__main__':
    sys.exit(main())
