#!/usr/bin/env python3

#TODO authors etc
import os
import argparse
import shutil

def clone_quasar(test_branch):
    os.system(f'git clone --recursive -b {test_branch} --depth=1 https://github.com/quasar-team/quasar.git')

def prepare_opcua_backend(opcua_backend, open62541_compat_branch):
    if opcua_backend == 'o6':
        os.system(f'./quasar.py enable_module open62541-compat {open62541_compat_branch}')
        os.system(f'./quasar.py set_build_config open62541_config.cmake')
    else:
        raise Exception("backend unsupported")

def build():
    os.system(f'./quasar.py build Release')

def run_and_dump_address_space():
    os.system('./.CI/travis/server_fixture.py --command_to_run uasak_dump')

def compare_with_nodeset(reference_ns):
    os.system(f'/opt/NodeSetTools/nodeset_compare.py {reference_ns} build/bin/dump.xml --ignore_nodeids StandardMetaData')

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--clone', action='store_true')
    parser.add_argument('--quasar_branch', default='master')
    parser.add_argument('--opcua_backend')
    parser.add_argument('--open62541_compat_branch')
    parser.add_argument('--design', default=None)
    parser.add_argument('--config', default=None)
    parser.add_argument('--compare_with_nodeset', default=None)

    args = parser.parse_args()

    if args.clone:
        clone_quasar(args.quasar_branch)

    prepare_opcua_backend(args.opcua_backend, args.open62541_compat_branch)

    if args.design:
        shutil.copyfile(args.design, 'Design/Design.xml')

    build()

    if args.config:
        shutil.copyfile(args.config, 'build/bin/config.xml')

    run_and_dump_address_space()
    if args.compare_with_nodeset:
        compare_with_nodeset(args.compare_with_nodeset)

    print('a')
    pass

if __name__ == "__main__":
    main()
