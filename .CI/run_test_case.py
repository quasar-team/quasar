#!/usr/bin/env python3

#TODO authors etc
import os
import argparse
import shutil
import sys
from colorama import Fore, Style

def invoke_and_check(cmd):
    print(f'{Fore.BLUE}{Style.BRIGHT}Will invoke{Style.RESET_ALL} {cmd}')
    ret_val = os.system(cmd)
    if ret_val != 0:
        raise Exception (f'Stopping because the command {cmd} returned wrong return value of {ret_val}')

def clone_quasar(test_branch):
    invoke_and_check(f'git clone --recursive -b {test_branch} --depth=1 https://github.com/quasar-team/quasar.git')
    os.chdir('quasar')

def prepare_opcua_backend(opcua_backend, open62541_compat_branch):
    if opcua_backend == 'o6':
        os.system(f'./quasar.py enable_module open62541-compat {open62541_compat_branch}')
        os.system(f'./quasar.py set_build_config open62541_config.cmake')
    elif opcua_backend == 'uasdk':
        invoke_and_check('./quasar.py set_build_config .CI/travis/build_configs/uasdk-eval.cmake')
    else:
        raise Exception(f"OPCUA backend {opcua_backend} was not recognized")

def build():
    invoke_and_check('./quasar.py build Release')

def run_and_dump_address_space():
    invoke_and_check('./.CI/travis/server_fixture.py --command_to_run uasak_dump')

def compare_with_nodeset(reference_ns):
    invoke_and_check(f'/opt/NodeSetTools/nodeset_compare.py {reference_ns} build/bin/dump.xml --ignore_nodeids StandardMetaData')

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--clone', action='store_true')
    parser.add_argument('--quasar_branch', default='master')
    parser.add_argument('--opcua_backend')
    parser.add_argument('--open62541_compat_branch')
    parser.add_argument('--design', default=None)
    parser.add_argument('--config', default=None)
    parser.add_argument('--compare_with_nodeset', default=None)
    parser.add_argument('--generate_all_devices', action='store_true')
    args = parser.parse_args()

    if args.clone:
        clone_quasar(args.quasar_branch)

    prepare_opcua_backend(args.opcua_backend, args.open62541_compat_branch)

    if args.design:
        shutil.copyfile(args.design, 'Design/Design.xml')

    if args.generate_all_devices:
        invoke_and_check('./quasar.py generate device --all')

    build()

    if args.config:
        shutil.copyfile(args.config, 'build/bin/config.xml')

    run_and_dump_address_space()
    if args.compare_with_nodeset:
        compare_with_nodeset(args.compare_with_nodeset)

    print('a')
    pass

if __name__ == "__main__":
    try:
        main()
    except Exception as ex:
        print(f'{Fore.RED}Caught exception{Style.RESET_ALL} {str(ex)}, returning as failed')
        sys.exit(1)
