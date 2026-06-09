#!/usr/bin/env python3
'''
run_clang_tidy.py

Drive clang-tidy across quasar's hand-written C++ translation units. Reads the
compilation database that CMake emits as build/compile_commands.json, filters
down to TUs we actually maintain (skipping generated, vendored, and submodule
code), and reports per-check counts.

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>
@copyright:  2026 CERN
@license:    BSD-2-Clause
@contact:    quasar-developers@cern.ch
'''

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed
from colorama import Fore, Style, init as colorama_init

colorama_init()

REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
BUILD_DIR = os.path.join(REPO, 'build')
COMPILE_DB = os.path.join(BUILD_DIR, 'compile_commands.json')

# Module directories we own. Only sources under these roots are analysed.
OWNED_DIRS = ('Server/src', 'Server/include',
              'AddressSpace/src', 'AddressSpace/include',
              'Common/src', 'Common/include',
              'Meta/src', 'Meta/include',
              'Device/src', 'Device/include',
              'CalculatedVariables/src', 'CalculatedVariables/include')

# Substrings that, if present in a TU path, cause it to be skipped. Generated
# code and vendored third-party live here.
SKIP_SUBSTRINGS = ('/build/', '/generated/', '/LogIt/', '/muparser/',
                   'Base_All.cpp', 'AddressSpaceClasses.cpp',
                   'ASInformationModel.cpp', 'SourceVariables.cpp')

CHECK_TAG_RE = re.compile(r'\[([a-zA-Z0-9\-,_]+)\]\s*$')


def find_clang_tidy():
    explicit = os.environ.get('CLANG_TIDY')
    if explicit:
        return explicit
    return shutil.which('clang-tidy')


def load_tus():
    with open(COMPILE_DB) as f:
        entries = json.load(f)
    result = []
    for entry in entries:
        src = entry['file']
        if not src.startswith(REPO + os.sep):
            continue
        rel = os.path.relpath(src, REPO)
        if not any(rel.startswith(d + os.sep) for d in OWNED_DIRS):
            continue
        if any(s in src for s in SKIP_SUBSTRINGS):
            continue
        result.append(src)
    return sorted(set(result))


def run_tidy(tu, tidy_bin, fix):
    cmd = [tidy_bin, '--quiet', '-p', BUILD_DIR]
    if fix:
        cmd.append('--fix')
    cmd.append(tu)
    proc = subprocess.run(cmd, capture_output=True, text=True)
    return tu, proc.returncode, proc.stdout, proc.stderr


def main():
    parser = argparse.ArgumentParser(description='Run clang-tidy over quasar sources.')
    parser.add_argument('--fix', action='store_true', help='apply auto-fixes in place')
    parser.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    parser.add_argument('--limit', type=int, default=0, help='only process the first N TUs')
    parser.add_argument('--filter', default='', help='substring: only run on TUs whose path contains this')
    parser.add_argument('--list', action='store_true', help='list selected TUs and exit')
    parser.add_argument('--no-fail', action='store_true',
                        help='exit 0 even when findings are present (useful for advisory CI runs)')
    args = parser.parse_args()

    tidy_bin = find_clang_tidy()
    if tidy_bin is None:
        print(Fore.RED + 'clang-tidy not found on PATH.' + Style.RESET_ALL)
        print('  install clang-tools-extra (Alma/RHEL/Fedora) or clang-tidy (Debian/Ubuntu)')
        print('  override with:  CLANG_TIDY=/path/to/clang-tidy .CI/run_clang_tidy.py')
        sys.exit(2)

    if not os.path.exists(COMPILE_DB):
        print(Fore.RED + 'compile_commands.json missing at ' + COMPILE_DB + Style.RESET_ALL)
        print('Build with CMAKE_EXPORT_COMPILE_COMMANDS=ON, then:  ./quasar.py prepare_build')
        sys.exit(2)

    tus = load_tus()
    if args.filter:
        tus = [t for t in tus if args.filter in t]
    if args.limit:
        tus = tus[:args.limit]

    if args.list:
        for t in tus:
            print(os.path.relpath(t, REPO))
        return 0

    print(Fore.CYAN + Style.BRIGHT + 'clang-tidy: ' + tidy_bin + Style.RESET_ALL)
    print('TUs: ' + str(len(tus)) + '  jobs: ' + str(args.jobs) +
          ('  (fix mode)' if args.fix else ''))
    print()

    tus_with_findings = 0
    per_check = {}
    with ProcessPoolExecutor(max_workers=args.jobs) as ex:
        futures = {ex.submit(run_tidy, tu, tidy_bin, args.fix): tu for tu in tus}
        for fut in as_completed(futures):
            tu, _rc, out, _err = fut.result()
            if not out.strip():
                continue
            tus_with_findings += 1
            rel = os.path.relpath(tu, REPO)
            print(Fore.YELLOW + Style.BRIGHT + '--- ' + rel + ' ---' + Style.RESET_ALL)
            print(out.rstrip())
            print()
            for line in out.splitlines():
                m = CHECK_TAG_RE.search(line)
                if not m:
                    continue
                for chk in m.group(1).split(','):
                    per_check[chk] = per_check.get(chk, 0) + 1

    print(Fore.MAGENTA + Style.BRIGHT + '==== summary ====' + Style.RESET_ALL)
    print('TUs with findings: ' + str(tus_with_findings) + ' / ' + str(len(tus)))
    if per_check:
        print('Per check:')
        width = max(len(k) for k in per_check)
        for chk, n in sorted(per_check.items(), key=lambda kv: (-kv[1], kv[0])):
            print('  ' + chk.ljust(width) + '  ' + str(n))
    else:
        print(Fore.GREEN + 'clean' + Style.RESET_ALL)
    return 0 if (args.no_fail or not tus_with_findings) else 1


if __name__ == '__main__':
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print(Fore.YELLOW + 'interrupted' + Style.RESET_ALL)
        sys.exit(130)
