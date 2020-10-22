#!/usr/bin/env python3
'''
run_all_travis_tests_locally.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

@license:
Copyright (c) 2015, CERN.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS  OR
IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES  OF  MERCHANTABILITY
AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
DAMAGES (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY  OF  LIABILITY,
WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN
ANY WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import yaml
import os
import datetime
import math
import pygit2
import sys
from colorama import Fore, Style

def main():
    repo = pygit2.Repository("../../.")
    travis_pull_request_branch = repo.head.shorthand
    print('Current branch is:', travis_pull_request_branch)

    yaml_file = open('../../.travis.yml')
    yaml_repr = yaml.load(yaml_file)

    job_results = {}

    for job in yaml_repr['jobs']['include']:
        print(Fore.GREEN + "Now at job: " + job['name'] + Style.RESET_ALL)
        print('job name: ' + job['name'])
        cmd_list = job['script']
        for cmd in cmd_list:
            print('Lets execute: ' + cmd)
            t0 = datetime.datetime.now().timestamp()
            return_code = os.system('export TRAVIS_PULL_REQUEST_BRANCH={0};'.format(travis_pull_request_branch, cmd))
            t1 = datetime.datetime.now().timestamp()
            job_result = {}
            job_result['return_code'] = return_code
            job_result['duration'] = math.ceil(t1-t0)
            job_results[job['name']] = job_result

    print ('\n\n----- Final results -----\n\n')

    print('{0:40} | {1:8} | {2:15}'.format('test name', 'ret code', 'time spent [s]'))
    print('{0:-<40} + {1:-<8} + {2:-<15}'.format('','',''))
    for name in job_results:
        job_result = job_results[name]
        print('{0:40} | {1:8} | {2:15}'.format(name, job_result['return_code'], job_result['duration']))

if __name__ == "__main__":
    main()
