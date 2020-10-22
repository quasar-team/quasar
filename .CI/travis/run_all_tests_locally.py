import yaml
import os
import datetime
import math
from colorama import Fore, Style

travis_pull_request_branch = 'OPCUA-2022_CI_test_locally'

yaml_file = open('../../.travis.yml')
yaml = yaml.load(yaml_file)

job_results = {}

for job in yaml['jobs']['include']:
    print(Fore.GREEN + "Now at job: " + job['name'] + Style.RESET_ALL)
    print('job name: ' + job['name'])
    cmd_list = job['script']
    for cmd in cmd_list:
        print('Lets execute: ' + cmd)
        t0 = datetime.datetime.now().timestamp()
        return_code = os.system('export TRAVIS_PULL_REQUEST_BRANCH={0}; '.format(travis_pull_request_branch, cmd))
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
