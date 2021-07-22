#!/usr/bin/env python3

#TODO authors etc
import os
import argparse

def clone_quasar(test_branch):
    os.system(f'git clone --recursive -b {test_branch} --depth=1 https://github.com/quasar-team/quasar.git')

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('--clone', action='store_true')
    parser.add_argument('--quasar_branch', default='master')


    args = parser.parse_args()

    if args.clone:
        clone_quasar(args.quasar_branch)

    print('a')
    pass

if __name__ == "__main__":
    main()
