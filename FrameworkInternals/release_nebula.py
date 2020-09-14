#!/usr/bin/env python3
# encoding: utf-8

'''
release_nebula.py

@author:     Piotr Nikiel <piotr@nikiel.info>
@author:     Damian Abalo Miron <damian.abalo@cern.ch>

@copyright:  2015 CERN

@license:

Copyright (c) 2015, CERN, Universidad de Oviedo.
All rights reserved.

Redistribution and  use in  source and  binary forms, with  or  without modification, are  permitted
provided that the following conditions are met:
  1. Redistributions of source  code must retain the above copyright notice, this list of conditions
     and the following disclaimer.
  2. Redistributions  in  binary  form  must  reproduce  the  above  copyright  notice, this list of
     conditions and the following  disclaimer in  the documentation  and/or other materials provided
     with the distribution.

THIS  SOFTWARE IS  PROVIDED  BY THE  COPYRIGHT HOLDERS  AND CONTRIBUTORS "AS IS" AND  ANY EXPRESS OR
IMPLIED  WARRANTIES, INCLUDING,  BUT NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF MERCHANTABILITY AND
FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE  DISCLAIMED.  IN NO  EVENT  SHALL THE  COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL
DAMAGES (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

from colorama import Fore, Style
import argparse
import re
import os
import sys

import pygit2

import quasar_basic_utils

def assert_correct_branch():
    pass
    # TODO

def assert_tag_valid(tag):
    '''Makes sure given tag adheres to the schema'''
    given_tag_regex = re.compile(r"[A-Z]\d+")
    match = given_tag_regex.match(tag)
    if match is None:
        raise Exception("Given tag is in wrong format. Correct examples: A1, M42, K133")
    print("Tag in valid format:    " + Fore.GREEN + "PASS" + Style.RESET_ALL)

def assert_correct_workdir():
    if not os.path.isfile("quasar.py"):
        raise Exception("You are running me from wrong directory... (no quasar.py here!)")
    print("Correct work directory: " + Fore.GREEN + "PASS" + Style.RESET_ALL)

def assert_all_files_committed():
    '''Raises for any non-versioned, modified or staged files without a commit.'''
    repo = pygit2.Repository(".")
    status = repo.status()
    file_names = status.keys()
    for file_name in file_names:
        if not repo.path_is_ignored(file_name):
            raise Exception("Ooops... seems you've got uncommitted files, check with git status.")
    print("All files committed:    " + Fore.GREEN + "PASS" + Style.RESET_ALL)

def store_version(version):
    '''Puts quasar version in respective files'''
    f_txt = open(os.path.sep.join(['Design', 'quasarVersion.txt']), 'w')
    f_txt.write(version)

    f_cpp = open(os.path.sep.join(['Server', 'include', 'QuasarVersion.h']), 'w')
    f_cpp.write('#define QUASAR_VERSION_STR "{0}"'.format(version))

def main():
    print(Fore.BLUE + "Note: you are about to release quasar-nebula!" + Style.RESET_ALL)
    parser = argparse.ArgumentParser()
    parser.add_argument("tag")

    args = parser.parse_args()
    given_tag = args.tag

    assert_correct_branch()
    assert_tag_valid(given_tag)
    assert_correct_workdir()
    #assert_all_files_committed()

    version = "nebula." + given_tag
    store_version(version)

    os.system('./quasar.py create_release')

    os.system('git commit -a -m "by release_nebula.py for given tag: {0}"'.format(given_tag))

if __name__ == "__main__":
    try:
        main()
    except:
        quasar_basic_utils.quasaric_exception_handler()
        sys.exit(1)
