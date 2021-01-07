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

import argparse
import yaml
import os
import datetime
import math
import pygit2
import sys
from colorama import Fore, Style
import threading

def worker(lock, jobs_to_do, travis_pull_request_branch, job_results):
    while True:
        with lock:
            if len(jobs_to_do) == 0:
                return
            job = jobs_to_do.pop(0)
        print(Fore.GREEN + "Now at job: " + job['name'] + Style.RESET_ALL)
        cmd_list = job['script']
        for cmd in cmd_list:
            print('Lets execute: ' + cmd)
            t0 = datetime.datetime.now().timestamp()
            return_code = os.system('export TRAVIS_PULL_REQUEST_BRANCH={0}; {1}'.format(travis_pull_request_branch, cmd))
            t1 = datetime.datetime.now().timestamp()
            job_result = {}
            job_result['return_code'] = return_code
            job_result['duration'] = math.ceil(t1-t0)
        with lock:
            job_results[job['name']] = job_result


def main():
    repo = pygit2.Repository("../../.")
    travis_pull_request_branch = repo.head.shorthand
    print('Current branch is:', travis_pull_request_branch)

    yaml_file = open('../../.travis.yml')
    yaml_repr = yaml.load(yaml_file)
    job_names_in_yaml = [job['name'] for job in yaml_repr['jobs']['include']]
    print('note: jobs declared in the yaml are:', job_names_in_yaml)

    parser = argparse.ArgumentParser()
    parser.add_argument("--select-jobs", help="A comma-separated list of jobs to execute instead of all jobs", default = ','.join(job_names_in_yaml))
    parser.add_argument("--no-term-reset", help="Dont reset the terminal", action='store_true')
    parser.add_argument("--num-workers", default=3, type=int, help="Number of jobs to run concurrently.")
    args = parser.parse_args()

    job_names_selected = args.select_jobs.split(',')
    jobs_to_do = [job for job in yaml_repr['jobs']['include'] if job['name'] in job_names_selected]
    print('The following jobs were selected to be executed:', '\n'.join([job['name'] for job in jobs_to_do]))

    job_results = {}

    lock = threading.Lock()
    threads = []
    for i in range(args.num_workers):
        w = threading.Thread(target=worker, args=(lock, jobs_to_do, travis_pull_request_branch, job_results,))
        w.start()
        threads.append(w)
    for t in threads:
        t.join()

    if not args.no_term_reset:
        os.system('reset')  # docker messes with terminal ...
        print ('... terminal was reset because docker messed it usually.')
    print ('\n\n----- Final results (test branch: {0}) -----\n\n'.format(travis_pull_request_branch))

    print('{0:40} | {1:8} | {2:15}'.format('test name', 'ret code', 'time spent [s]'))
    print('{0:-<40} + {1:-<8} + {2:-<15}'.format('','',''))
    for name in job_results:
        job_result = job_results[name]
        return_code = job_result['return_code']
        if return_code != 0:
            return_code = Fore.RED + '{0:8}'.format(return_code) + Style.RESET_ALL
        else:
            return_code = Fore.GREEN + '{0:8}'.format(return_code) + Style.RESET_ALL
        print('{0:40} | {1} | {2:15}'.format(name, return_code, job_result['duration']))

if __name__ == "__main__":
    main()
