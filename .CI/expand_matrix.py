#!/usr/bin/env python3
"""Expand .CI/test_cases/manifest.json into the GitHub Actions job matrix.

One declarative manifest -> one strategy.matrix, instead of hand-written jobs.
The SAME manifest is read by run_test_case.py --case (the driver), so each test
case is declared exactly once. Emits {"include": [ ...cells... ]} on stdout, for

    matrix: ${{ fromJSON(needs.matrix.outputs.include) }}

A cell is (case x backend x os x compiler). Its CI image is
    ghcr.io/<owner>/quasar-ci:<os>-<backend>[-<toolkit_version>]
computed here as the `image` field (the workflow prepends ghcr.io/<owner>/...).

Filters (compose; also read from env so the workflow can pass them as vars):
    --backends o6,uasdk     QUASAR_CI_BACKENDS   restrict to these backends
    --os alma9,alma10       QUASAR_CI_OS         restrict to these operating systems
    --tier pr,nightly       QUASAR_CI_TIERS      restrict to these tiers (default tier: pr)

Modes:
    (default)         emit the matrix JSON on one line (for $GITHUB_OUTPUT)
    --print           human-readable cell list grouped by case + total
    --assert-count N  exit non-zero unless exactly N cells expand (migration guard)
    --check-paths     verify every design/config/oracle/device_sources path exists

Stdlib only (json) so it runs in any CI image without extra packages.
"""
import argparse
import json
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
CASES_DIR = os.path.join(HERE, 'test_cases')
MANIFEST = os.path.join(CASES_DIR, 'manifest.json')

_CC = {'gcc': '', 'clang': 'clang'}
_CXX = {'gcc': '', 'clang': 'clang++'}


def load():
    with open(MANIFEST) as handle:
        return json.load(handle)


def _image(os_name, backend, toolkit_version):
    """ghcr.io/<owner>/quasar-ci:<image>. o6 needs no toolkit; uasdk pins a version."""
    if backend == 'uasdk' and toolkit_version:
        return '{}-{}-{}'.format(os_name, backend, toolkit_version)
    return '{}-{}'.format(os_name, backend)


def _cell(manifest, case, backend, os_name, compiler, toolkit_version, tier,
          arch='x86_64', compat_branch=None, advisory=False, build_config=None):
    name = case['name']
    parts = [name, backend, os_name] + ([compiler] if compiler != 'gcc' else [])
    if toolkit_version:
        parts.append(toolkit_version)
    if build_config:
        import os.path
        parts.append(os.path.splitext(os.path.basename(build_config))[0])
    if compat_branch:
        # Off-pin compat ref: mark the label so it never collides with the pinned cell
        # (dedup is by label) and stays invisible to cross_backend_parity's label regex.
        parts.append('compat-' + compat_branch.replace('/', '-'))
    image = _image(os_name, backend, toolkit_version)
    runner = 'ubuntu-latest'
    if arch != 'x86_64':
        # arch-tagged image (built natively on an arch runner) + arch label suffix + arch runner.
        parts.append(arch)
        image = '{}-{}'.format(image, arch)
        runner = 'ubuntu-24.04-arm' if arch == 'arm64' else 'ubuntu-latest'
    return {
        'label': '-'.join(parts),
        'case': name,
        'backend': backend,
        'os': os_name,
        'compiler': compiler,
        'toolkit_version': toolkit_version or '',
        'arch': arch,
        'runner': runner,
        'tier': tier,
        'image': image,
        'cc': _CC[compiler],
        'cxx': _CXX[compiler],
        'compat_branch': compat_branch or manifest['backends'].get(backend, {}).get('compat_branch', ''),
        'advisory': advisory,
        'build_config': build_config or '',
    }


def expand(manifest):
    """Flatten cases x backends x (uasdk toolkit versions) x os/compiler, with tiers.

    A backend listed in manifest['toolkits'] (uasdk) fans out into one cell per
    toolkit version, each carrying that version's tier; a backend without toolkit
    entries (o6) yields a single cell at the cell's base tier."""
    default_os = manifest.get('default_os', 'alma9')
    # Arches the PRIMARY OS (default_os) is fully tested on at PR. x86_64 is the baseline; listing
    # arm64 here gives the full per-case PR sweep on native arm too (only the PR-default uasdk
    # version, since that is the only version with arch images). Defaults to x86_64-only.
    default_arches = manifest.get('default_arches', ['x86_64'])
    nightly_oses = manifest.get('nightly_os', [])
    toolkits = manifest.get('toolkits', {})
    # Representative subset run on NON-default axes (extended OS, extra compiler, extra arch, and
    # non-default toolkit versions): those axes are framework/SDK/platform-level deltas whose
    # pass/fail signal is case-INDEPENDENT, so only a few broad-surface cases need re-run there.
    # Empty/absent => keep the full case set (current behaviour).
    representative = set(manifest.get('representative_cases', []))

    def in_rep(case):
        return (not representative) or case['name'] in representative

    def emit(case, backend, os_name, compiler, base_tier, force_tier=None, only_versions=None, arch='x86_64',
             compat_branch=None, advisory=False, build_config=None):
        # force_tier overrides each toolkit version's own tier (used for nightly_os, so a
        # PR-tier version like 1.8.9 still lands at nightly on the extended OS).
        # only_versions (set) restricts which uasdk toolkit versions are emitted here.
        versions = toolkits.get(backend)
        if versions:
            # A toolkit entry may carry its own 'cases' list (e.g. a version on the way
            # out runs only a smoke case); absent = the full case set.
            return [_cell(manifest, case, backend, os_name, compiler,
                          tk['version'], force_tier or tk.get('tier', base_tier), arch=arch,
                          compat_branch=compat_branch, advisory=advisory, build_config=build_config)
                    for tk in versions
                    if (not only_versions or tk['version'] in only_versions)
                    and (not tk.get('cases') or case['name'] in tk['cases'])]
        return [_cell(manifest, case, backend, os_name, compiler, None, force_tier or base_tier, arch=arch,
                      compat_branch=compat_branch, advisory=advisory, build_config=build_config)]

    cells = []
    for case in manifest['cases']:
        case_tier = case.get('tier', 'pr')
        for backend in case.get('backends', []):
            for arch in default_arches:
                # default_os x86_64: every toolkit version fans out per its own 'cases'
                # list (absent = full case set) at its own tier. On an alt default arch
                # (e.g. arm64) a version reaches the sweep only if it declares an image
                # for that arch (toolkit 'arches', default x86_64-only), each at its own
                # tier -- so the PR gate can move to a version whose alt-arch image
                # doesn't exist yet without breaking the alt-arch cells.
                if arch == 'x86_64':
                    only_v = None
                else:
                    only_v = {tk['version'] for tk in toolkits.get('uasdk', [])
                              if arch in tk.get('arches', ['x86_64'])}
                cells += emit(case, backend, default_os, 'gcc', case_tier,
                              only_versions=only_v, arch=arch)
        for extra in case.get('extra_cells', []):
            # A per-cell 'version' pins this extra_cell to one uasdk toolkit version instead of
            # fanning out to all of them (keeps a single alma10 smoke at the default version).
            pin = {extra['version']} if extra.get('version') else None
            cells += emit(case, extra['backend'], extra.get('os', default_os),
                          extra.get('compiler', 'gcc'), extra.get('tier', case_tier),
                          only_versions=pin,
                          compat_branch=extra.get('compat_branch'),
                          advisory=extra.get('advisory', False),
                          build_config=extra.get('build_config'))
    # Extended OSes (e.g. alma10): mirror the FULL case set at nightly so PR stays lean on default_os.
    # A secondary EL major is a first-class platform target, so EVERY case runs there -- deliberately
    # NOT narrowed by representative_cases (unlike arm64 / the alt toolkit versions). An entry may be a
    # bare string "alma10" (full version sweep, legacy) OR {"os": "alma10", "versions": ["1.8.9"]} to
    # mirror the full case set against only the listed uasdk versions on that EL.
    for entry in nightly_oses:
        os_name = entry['os'] if isinstance(entry, dict) else entry
        only_v = set(entry.get('versions', [])) if isinstance(entry, dict) else None
        for case in manifest['cases']:
            for backend in case.get('backends', []):
                cells += emit(case, backend, os_name, 'gcc', 'nightly',
                               force_tier='nightly', only_versions=only_v)
    # Extra compilers (e.g. clang): run the FULL case set with that compiler at nightly, on the
    # listed backends/os. Deliberately NOT narrowed by representative_cases -- a compiler can break
    # on case-specific generated C++ (templates/headers), so every case is compiled under it.
    for nc in manifest.get('nightly_compilers', []):
        for case in manifest['cases']:
            for backend in nc.get('backends', []):
                if backend in case.get('backends', []):
                    cells += emit(case, backend, nc['os'], nc['compiler'], 'nightly', force_tier='nightly')
    # Extra architectures (e.g. arm64): run the case set at nightly on a native arch runner with
    # arch-tagged images. 'versions' restricts which uasdk toolkit versions get an arch image
    # (we only build a subset of arch images); 'cases' optionally restricts the case set.
    for na in manifest.get('nightly_arches', []):
        arch = na['arch']
        arch_os = na.get('os', default_os)
        keep_versions = set(na.get('versions', []))
        keep_cases = set(na.get('cases', []))
        for case in manifest['cases']:
            if keep_cases and case['name'] not in keep_cases:
                continue
            if not in_rep(case):
                continue
            for backend in case.get('backends', []):
                versions = toolkits.get(backend)
                if versions:
                    for tk in versions:
                        if keep_versions and tk['version'] not in keep_versions:
                            continue
                        cells.append(_cell(manifest, case, backend, arch_os, 'gcc',
                                           tk['version'], 'nightly', arch=arch))
                else:
                    cells.append(_cell(manifest, case, backend, arch_os, 'gcc', None, 'nightly', arch=arch))
    # De-dup by label: explicit extra_cells (emitted above, first) win, preserving their tier
    # and compiler (e.g. default_design's PR alma10 smoke + the alma10/clang cell).
    deduped = {}
    for c in cells:
        deduped.setdefault(c['label'], c)
    return list(deduped.values())


def _csv_env(cli_value, env_name):
    raw = cli_value if cli_value is not None else os.environ.get(env_name)
    if not raw:
        return None
    return {item.strip() for item in raw.split(',') if item.strip()}


def apply_filters(cells, backends, oses, tiers, versions=None, cases=None, arches=None):
    if arches:
        cells = [c for c in cells if c['arch'] in arches]
    if cases:
        cells = [c for c in cells if c['case'] in cases]
    if backends:
        cells = [c for c in cells if c['backend'] in backends]
    if oses:
        cells = [c for c in cells if c['os'] in oses]
    if tiers:
        cells = [c for c in cells if c['tier'] in tiers]
    if versions:
        # Only narrows version-bearing (uasdk) cells; version-less cells (o6) pass through,
        # so this composes with --backends (e.g. uasdk + 1.8.9,2.0.2 -> just those cells).
        cells = [c for c in cells if (not c['toolkit_version']) or c['toolkit_version'] in versions]
    return cells


def check_paths(manifest):
    missing = []
    for case in manifest['cases']:
        for key in ('design', 'config', 'oracle'):
            rel = case.get(key)
            if rel and not os.path.exists(os.path.join(CASES_DIR, rel)):
                missing.append('{}.{} -> {}'.format(case['name'], key, rel))
        device_sources = case.get('device_sources')
        if device_sources and not os.path.isdir(os.path.join(CASES_DIR, device_sources)):
            missing.append('{}.device_sources -> {}/'.format(case['name'], device_sources))
    return missing


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--print', action='store_true', dest='do_print')
    parser.add_argument('--assert-count', type=int)
    parser.add_argument('--check-paths', action='store_true')
    parser.add_argument('--backends')
    parser.add_argument('--os', dest='oses')
    parser.add_argument('--tier', dest='tiers')
    parser.add_argument('--versions', help='restrict uasdk toolkit versions (e.g. 1.8.9,2.0.2); o6 cells unaffected')
    parser.add_argument('--cases', help='restrict to these case names (e.g. default_design)')
    parser.add_argument('--arches', help='restrict to these CPU arches (x86_64,arm64)')
    args = parser.parse_args()

    manifest = load()

    if args.check_paths:
        missing = check_paths(manifest)
        if missing:
            print('manifest references missing files:', file=sys.stderr)
            for entry in missing:
                print('  ' + entry, file=sys.stderr)
            sys.exit(1)
        print('paths OK ({} cases)'.format(len(manifest['cases'])))
        return

    cells = apply_filters(
        expand(manifest),
        _csv_env(args.backends, 'QUASAR_CI_BACKENDS'),
        _csv_env(args.oses, 'QUASAR_CI_OS'),
        _csv_env(args.tiers, 'QUASAR_CI_TIERS'),
        _csv_env(args.versions, 'QUASAR_CI_VERSIONS'),
        _csv_env(args.cases, 'QUASAR_CI_CASES'),
        _csv_env(args.arches, 'QUASAR_CI_ARCHES'),
    )

    if args.assert_count is not None and len(cells) != args.assert_count:
        print('cell count {} != expected {}'.format(len(cells), args.assert_count), file=sys.stderr)
        sys.exit(1)

    if args.do_print:
        by_case = {}
        for cell in cells:
            by_case.setdefault(cell['case'], []).append(cell['label'])
        for case, labels in by_case.items():
            print('{} ({}):'.format(case, len(labels)))
            for label in labels:
                print('    ' + label)
        print('\ntotal cells: {}'.format(len(cells)))
    else:
        print(json.dumps({'include': cells}))


if __name__ == '__main__':
    main()
