import os
import re
import sys

QUASAR_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SCAN_ROOTS = ('.github', '.CI')
WINDOWS_WORKFLOW = os.path.join('.github', 'workflows', 'windows.yml')

ORACLE_NAME = 'nodesettools'
ORACLE_ACQUISITION_SITES = (
    WINDOWS_WORKFLOW,
    os.path.join('.CI', 'docker', 'Dockerfile.alma9-o6'),
    os.path.join('.CI', 'docker', 'Dockerfile.alma10-o6'),
)

FULL_COMMIT_RE = re.compile(r'(?<![0-9a-zA-Z])[0-9a-f]{40}(?![0-9a-zA-Z])')
REF_FLAG_RE = re.compile(r'(?:^|\s)(-b|--branch)[= ]\s*([^\s\'"]+)')
QUOTED_SPAN_RE = re.compile(r"'[^']*'|\"[^\"]*\"")
GIT_COMMAND_RE = re.compile(r'\bgit\b')

UASAK_DUMP_BUILD_STEP_RE = re.compile(r'^\s+- name: Build uasak_dump\b')
LIBXSD_PATCH_STEP_RE = re.compile(r'^\s+- name: Patch CodeSynthesis libxsd\b')
JOBS_KEY_RE = re.compile(r'^jobs:\s*$')
JOB_ID_RE = re.compile(r'^  ([A-Za-z0-9_-]+):\s*$')

WHY_ONE_PIN = (
    'every site that acquires the NodeSetTools nodeset_compare oracle must name the SAME commit: '
    'the Windows lanes fetch it per cell and the Linux lanes bake it into the CI images, so a '
    'half-edit lets the two platforms judge the same address-space dump with different comparator '
    'code while every job stays green'
)
WHY_NO_MUTABLE_REF = (
    'the oracle must be acquired by exact commit, never by branch or tag: a lightweight tag can be '
    'moved upstream and would silently change what CI compares against, with no commit in this repo. '
    'git clone -b accepts branch and tag names only and never a commit, so the pinned form is '
    'git init + git remote add origin + git fetch --depth=1 origin <commit> + git checkout --detach FETCH_HEAD'
)
WHY_BUILD_BEFORE_PATCH = (
    'the uasak_dump cache key derives nothing from libxsd content, and the o6 and uasdk lanes share '
    'one key, so uasak_dump must be built BEFORE the uasdk lane rewrites the CodeSynthesis libxsd '
    'headers in place. With the patch first, the two lanes compute an identical key while compiling '
    'against different headers and whichever lane wins the race decides what every cell restores'
)


def is_probably_text(path):
    try:
        with open(path, 'rb') as handle:
            return b'\0' not in handle.read(8192)
    except OSError:
        return False


def scanned_files():
    for root_name in SCAN_ROOTS:
        root = os.path.join(QUASAR_ROOT, root_name)
        for dirpath, dirnames, filenames in os.walk(root):
            dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
            for filename in filenames:
                path = os.path.join(dirpath, filename)
                if is_probably_text(path):
                    yield os.path.relpath(path, QUASAR_ROOT), path


def oracle_lines():
    for rel, path in scanned_files():
        with open(path, 'r', encoding='utf-8', errors='replace') as handle:
            for number, line in enumerate(handle, 1):
                if ORACLE_NAME in line.lower():
                    yield rel, number, line.rstrip('\n')


def check_single_oracle_pin():
    pins_by_site = {}
    for rel, number, line in oracle_lines():
        for sha in FULL_COMMIT_RE.findall(line):
            pins_by_site.setdefault(sha, []).append('%s:%d' % (rel, number))

    if not pins_by_site:
        print('FAIL: no 40-hex commit found on any line mentioning NodeSetTools under %s. %s'
              % (' or '.join(SCAN_ROOTS), WHY_ONE_PIN))
        return False

    if len(pins_by_site) != 1:
        print('FAIL: the NodeSetTools oracle is pinned to %d different commits - %s' % (len(pins_by_site), WHY_ONE_PIN))
        for sha in sorted(pins_by_site):
            print('        %s  <-  %s' % (sha, ', '.join(pins_by_site[sha])))
        return False

    pin = next(iter(pins_by_site))
    sites_with_pin = {location.split(':')[0] for location in pins_by_site[pin]}
    missing = [site for site in ORACLE_ACQUISITION_SITES if site not in sites_with_pin]
    if missing:
        print('FAIL: these known oracle acquisition sites no longer carry the pinned commit: %s. %s. '
              'If a site was legitimately removed, drop it from ORACLE_ACQUISITION_SITES in this test.'
              % (', '.join(missing), WHY_ONE_PIN))
        return False

    print('OK: NodeSetTools oracle pinned to %s at %d site(s): %s'
          % (pin, len(pins_by_site[pin]), ', '.join(sorted(pins_by_site[pin]))))
    return True


def check_no_mutable_oracle_ref():
    offenders = []
    for rel, number, line in oracle_lines():
        outside_messages = QUOTED_SPAN_RE.sub(' ', line)
        if not GIT_COMMAND_RE.search(outside_messages):
            continue
        for flag, ref in REF_FLAG_RE.findall(outside_messages):
            if not FULL_COMMIT_RE.fullmatch(ref):
                offenders.append('%s:%d acquires NodeSetTools with %s %s' % (rel, number, flag, ref))

    if offenders:
        print('FAIL: %s' % WHY_NO_MUTABLE_REF)
        for offender in offenders:
            print('        %s' % offender)
        return False

    print('OK: no NodeSetTools acquisition uses a branch or tag ref')
    return True


def windows_workflow_jobs():
    path = os.path.join(QUASAR_ROOT, WINDOWS_WORKFLOW)
    with open(path, 'r', encoding='utf-8', errors='replace') as handle:
        lines = handle.read().splitlines()

    jobs_at = next((i for i, line in enumerate(lines) if JOBS_KEY_RE.match(line)), None)
    if jobs_at is None:
        return None, lines

    starts = [(i, JOB_ID_RE.match(line).group(1)) for i, line in enumerate(lines)
              if i > jobs_at and JOB_ID_RE.match(line)]
    jobs = []
    for position, (start, job_id) in enumerate(starts):
        end = starts[position + 1][0] if position + 1 < len(starts) else len(lines)
        jobs.append((job_id, start, end))
    return jobs, lines


def check_uasak_dump_built_before_libxsd_patch():
    jobs, lines = windows_workflow_jobs()
    if jobs is None:
        print('FAIL: %s has no top-level jobs: key - this test cannot locate the uasdk job' % WINDOWS_WORKFLOW)
        return False

    patch_lines = [i for i, line in enumerate(lines) if LIBXSD_PATCH_STEP_RE.match(line)]
    if not patch_lines:
        print('OK: %s has no libxsd patch step, so the uasak_dump cache cannot straddle two header sets'
              % WINDOWS_WORKFLOW)
        return True

    ok = True
    for patch_at in patch_lines:
        job = next(((job_id, start, end) for job_id, start, end in jobs if start <= patch_at < end), None)
        if job is None:
            print('FAIL: %s:%d patches libxsd outside any job block' % (WINDOWS_WORKFLOW, patch_at + 1))
            ok = False
            continue
        job_id, start, end = job
        builds = [i for i in range(start, end) if UASAK_DUMP_BUILD_STEP_RE.match(lines[i])]
        if not builds:
            continue
        if min(builds) > patch_at:
            print('FAIL: job %s patches libxsd at %s:%d before building uasak_dump at %s:%d - %s'
                  % (job_id, WINDOWS_WORKFLOW, patch_at + 1, WINDOWS_WORKFLOW, min(builds) + 1,
                     WHY_BUILD_BEFORE_PATCH))
            ok = False
        else:
            print('OK: job %s builds uasak_dump (line %d) before patching libxsd (line %d)'
                  % (job_id, min(builds) + 1, patch_at + 1))
    return ok


results = [
    check_single_oracle_pin(),
    check_no_mutable_oracle_ref(),
    check_uasak_dump_built_before_libxsd_patch(),
]

if all(results):
    print('\nCI SUPPLY CHAIN TEST PASSED: all %d checks.' % len(results))
    sys.exit(0)
print('\n%d of %d checks FAILED.' % (sum(1 for r in results if not r), len(results)))
sys.exit(1)
