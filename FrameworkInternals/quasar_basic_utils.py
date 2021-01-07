'''
quasar_basic_utils.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2019 CERN

@license:
Copyright (c) 2019, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact: quasar-developers@cern.ch
'''

import os
import sys
import traceback
import pdb
import logging
from colorama import Fore, Back, Style

def extract_argument(inData, key):
    """If key is present in inData, will remove it and the following element from the list.
       Returns a tuple of the output list (i.e. after removal of two elements) and the value
       of the element (i.e. the second of the two)"""
    if key in inData:
        pos = inData.index(key)
        output = inData
        output.pop(pos)
        try:
            value = output.pop(pos)
        except IndexError:
            raise Exception('Argument {0} requires to be followed by a value. Run out of arguments.'.format(key))
        return (output, value)
    else:  # nothing to do, argument not present
        return (inData, None)

def quasaric_exception_handler():
    """Prints last exception in a nicer way (colors etc), plus let's run pdb to inspect it on the
       go"""
    extype, value, tb = sys.exc_info()
    print(Fore.RED + 'quasar tooling caught an exception of type {0} when executing '.format(
          type(value).__name__)
          + Fore.MAGENTA + ' '.join(sys.argv) + Style.RESET_ALL)
    print('Exception was: ' + Back.RED +  Fore.WHITE + str(value) + Style.RESET_ALL)
    traceback.print_exc()
    if os.getenv('QUASAR_RUN_PDB', False):
        print(Fore.RED
              + ('... running pdb now (if pdb shell is gone then maybe you want to repeat that '
                 'particular quasar command alone)') + Style.RESET_ALL)
        print(Fore.GREEN
              + 'remove QUASAR_RUN_PDB from your shell environment if you dont wish to run pdb'
              + Style.RESET_ALL)
        pdb.post_mortem(tb)
    else:
        print(Fore.GREEN
              + ("Export QUASAR_RUN_PDB to your environment if you wish to automatically start "
                 "Python debugger! (e.g. 'export QUASAR_RUN_PDB=1' in bash)") + Style.RESET_ALL)

def get_quasar_version(where=''):
    """Returns version of quasar deployed in the current project or anothe project if
       where is given"""
    version_f = open(os.path.join(where, 'Design', 'quasarVersion.txt'), 'r', encoding='utf-8')
    return version_f.readline().rstrip()

def print_quasar_version():
    """Prints currently deployed quasar version"""
    print("quasar version: " + Fore.GREEN + get_quasar_version() + Style.RESET_ALL)

def initialize_logging():
    """Initializes Python's logging"""
    log_levels = {
        'INF' : logging.INFO,
        'DBG' : logging.DEBUG
    }
    log_level_str = os.getenv('QUASAR_LOG_LEVEL', 'INF')
    if log_level_str not in log_levels:
        print(("The log level exported in the environment variable QUASAR_LOG_LEVEL of value {0} "
               "is invalid. Please supply a valid one from the set {1}").format(
                   log_level_str,
                   ', '.join(log_levels)))
        sys.exit(1)
    logging.basicConfig(level=log_levels[log_level_str])
    logging.debug('quasar tooling is running in log level %s',
                  logging.getLogger().getEffectiveLevel())

def yes_or_no(question):
    '''A simple user interaction asking for yes or no.'''
    while True:
        print(question+' type y or n; then enter   ')
        sys.stdout.flush()
        yn = input()
        if yn in ['y','n']:
            return yn

def print_logo(skip_top=0, skip_bottom=0):
    lines='''
                         ▄▄▄▄▄░░░
                    ▓▓▓▓▓▓▓▓▓▓▓▓▒                ▓▓▓▌      ▓▓▓▄    ▄▓▓▓▓▓▓▓▓▌▄
                  ░▓▓▓▓▓▓▓▓▓▓▓▌░                 ▓▓▓▌      ▌▓▓▌  ░▓▓▓▌▀▀▀▀▀▓▓▓▓
                 ▄▓▓▓▓▓▓▓▓▓▓▓░                   ▌▓▓▌      ▌▓▓▌  ▀▀▀▀      ▒▌▓▓
                ▒▓▓▓▓▓▓▓▓▓▓░      ▐▄▄▄▄▄▄▄▄░     ▓▓▓▌      ▌▓▓▒    ▄▄▓▓▓▓▓▓▓▓▓▓
  ░░           ▓▓▓▓▓▓▓▓▓▓▀      ░▓▓▓▓▓▓▓▓▓▓▓▀░   ▓▓▓▌      ▌▓▓▒  ▄▓▓▓▌▀▀▀░░▒▌▓▓
    ░▀▄░      ▓▓▓▓▓▓▓▓▓▌░      ▄▓▓▓▓▓▓▓▓▓▓▀      ▓▓▓▌     ▄▓▓▓▒ ▐▓▓▓▌     ▐█▓▓▓
       ░▀▄▄  ▓▓▓▓▓▓▓▓▓▀      ▄▓▓▓▓▓▓▓▓▓▓▀        ▐▓▓▓▓▓▓▓▓▓▓▓▓▒  ▀▓▓▓▓▄▄▓▓▓▓▓▓▓░
           ▀▓▓▓▓▓▓▓▓▓░     ▐▓▓▓▓▓▓▓▓▓▓▀            ▐▀▀▓▓▀▀░▀▀▀░   ░▀▀▓▓▓▀▀░ ▀▀▀▀
           ▓▓▓▓▓▓▓▓▌      ▄▓▓▓▓▓▓▓▓█▀
          ▄▓▓▓▓▓▓▓█▄     ▓▓▓▓▓▓▓▓█▌                  ▒▒▄▓▓▓▓
         ▐▓▓▓▓▓▓▓▀  ▀▓▄▄▓▓▓▓▓▓▓▓▓░  ▄▄▄▄▄▄░░           ░▓█▓▓░
         ▓▓▓▓▓▓▓▒     ▐█▓▓▓▓▓▓▓▀  ░████████▀   ░▄▄██▓▄   ▓█░
        ▄▓▓▓▓▓▓▀      ▓▓▓▓▓▓▓▓░  ▄███████▀  ▄▄█████████  ▐▓▄░
        ▓▓▓▓▓▓▌      ▓▓▓▓▓▓▓▀▀██████████░ ▄█████▀▀▀▀███  ▒█▓▓▒
       ▐▓▓▓▓▓▓░     ▐▓▓▓▓▓▓▌  ░███████░ ▄███▀░      ▐█▌ ▄▓█▓▓░
       ▐▓▓▓▓▓▀      █▓▓▓▓▓▀  ▐██████▀█████▀         ▓█ ░▓▌░
       ▒▓▓▓▓▓░     ▐▓▓▓▓▓▌  ░██████░ ▄████▄         ▀ ▐▓▓░
       ▒▓▓▓▓▓░     ▐█▓▓▓▓   ██████  ▄██░ ░▀██▄       ▄▓█▓▌
       ▀▓▓▓▓▓      ▐█▓▓█▌  ▐█████  ░█▌      ░▀██▄   ▓▓█▓▓▀
       ▐▓▓▓▓▓░     ▐▓▓▓█░  █████░  ▄▌          ▐▀███▌░ ▐
       ▐▓▓▓▓▓▄      ▓▓▓█  ▐████▌                ░▓▓▀██▄░
        ▀▓▓▓▓▓      ░▓▓▓░ ▐████▌          ▄▓░ ▐▓▓▓▓░ ▐▀██▄░
         ▓▓▓▓▓▌      ░▓▓▌  █████       ▄▄█▀ ▄▓▓▓▓▓▓▓    ░▀██▄░
          ▓▓▓▓▓▌       ▀▓▌ ▐█████▓▄▄▓███▀░▄▓▓█▓▓▓▓▀        ▐▀███▄
           ▀▓▓▓▓▓▄       ▐░  ▀▀█████▀▀░▄▓▓▓▀    ░             ▐▀███▄
            ▐▓▓▓▓▓▓▄               ▄▒▓▓▓▓▌                       ▐▀███▄
    '''
    for i in range(0,skip_top):
        print()
    print(Fore.BLUE)
    print(lines)
    print(Style.RESET_ALL)
    for i in range(0,skip_bottom):
        print()
