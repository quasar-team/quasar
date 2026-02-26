#!/usr/bin/env python
'''
server_fixture.py

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2020 CERN

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

import sys
import os
import subprocess
import time
import argparse
from colorama import Fore, Style

def print_msg(msg):
    """Prints fixture-specific messages in distinct colour"""
    print(Fore.CYAN + Style.BRIGHT + msg + Style.RESET_ALL)

def make_sure_the_bastard_dies(process):
    """Makes sure given process will either nicely or forcefully go see its maker"""
    process.terminate()
    time.sleep(1)
    remaining_attempts = 5
    while process.poll() is None:
        print_msg('Process is still alive... Remaining number of retries {0}'.format(
            remaining_attempts))
        if remaining_attempts == 0:
            print_msg("Sending ka-boom: TERMINATING this server by force, it probably hung up.")
            process.kill()
            process.poll()
            break
        remaining_attempts -= 1
        time.sleep(1)

def main():
    """Main program"""

    parser = argparse.ArgumentParser()
    parser.add_argument("--command_to_run",
        default=None,
        type=str,
        help='Supplementary command to run when server successfully started. Not mandatory.')
    args = parser.parse_args()

    os.chdir(os.path.sep.join(['build', 'bin']))

    executable = 'Release/OpcUaServer.exe' if sys.platform == 'win32' else './OpcUaServer'
    process = subprocess.Popen(executable)

    print_msg('Server process was run under PID: {0}'.format(process.pid))
    print_msg('Now waiting few seconds to let it spin up... ')
    time.sleep(5)

    if process.poll() is not None:
        # Bad: process already terminated. This is no good!
        print_msg("ERROR! Server process terminated prematurely! Test failed.")
        sys.exit(1) # Let now the CI that the test failed.
    else:
        # Ask kindly the process to terminate.
        print_msg("So far, so good - server process is up.")
        if args.command_to_run is not None:
            print_msg("Was requested to run this command: {0}".format(args.command_to_run))
            ret_code = os.system(args.command_to_run)
            print_msg("The command exited with code {0}. Nothing more to do.".format(ret_code))
            if ret_code != 0:
                make_sure_the_bastard_dies(process)
                sys.exit(1)
        else:
            print_msg("Will exit now as no command was requested to be run")
        make_sure_the_bastard_dies(process)
        if process.returncode == 0:
            print_msg('Server exited, return code zero.')
        elif sys.platform != 'win32': # Only for Linux, on Windows the return code is always 1 as far seen:
            print_msg('Server exited with invalid return code of {0}'.format(process.returncode))
            sys.exit(1)

if __name__ == "__main__":
    main()
