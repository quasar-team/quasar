import yaml
import os
from colorama import Fore, Style

travis_pull_request_branch = 'OPCUA-2022_CI_test_locally'

yaml_file = open('../../.travis.yml')
yaml = yaml.load(yaml_file)
for job in yaml['jobs']['include']:
    print(Fore.GREEN + "Now at job: " + job['name'] + Style.RESET_ALL)
    print('job name: ' + job['name'])
    cmd_list = job['script']
    for cmd in cmd_list:
        print('Lets execute: ' + cmd)
        os.system('export TRAVIS_PULL_REQUEST_BRANCH={0}; {1}'.format(travis_pull_request_branch, cmd))
