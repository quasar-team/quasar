#!/usr/bin/python
'''
quasar.py

@author:     Damian Abalo Miron <damian.abalo@cern.ch>
@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2015 CERN

@license:
Copyright (c) 2015, CERN, Universidad de Oviedo.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import os
import sys
import subprocess
import inspect
import traceback

this_script_path = os.path.abspath(sys.argv[0])
sys.path.insert(0, os.path.join(os.path.dirname(this_script_path), 'FrameworkInternals'))

from quasarCommands import printCommandList, extract_common_arguments
from quasarCommands import getCommands

# args starts from the command name (e.g. 'build') and skips the common arguments (e.g. 'project_binary_dir')
(args, project_binary_dir) = extract_common_arguments(sys.argv[1:])  # 1: to skip the script name given by the operating system
if project_binary_dir is None:
	project_binary_dir = os.getcwd()

if len(args) < 1:
        print 'The script was run without specifying what to do. Here are available commands:'
        printCommandList()
        sys.exit(1)

        
matched_commands = filter(lambda x: x[0] == args[0:len(x[0])], getCommands())
if len(matched_commands)>1:
        raise Exception ('It was ambigious to determine the command to run.')
if len(matched_commands)<1:
       	print 'Sorry, no such command. These are available:'
       	printCommandList()
       	sys.exit(1)
matched_command = matched_commands[0]


if '-h' in sys.argv or '--help' in sys.argv:
	help(matched_command[1])
	sys.exit(0)


command_invocation_length = len(matched_command[0])
real_args = args[command_invocation_length:]

function_arg_spec = inspect.getargspec( matched_command[1] ).args # inspect the arguments of callable
pass_args = []

order_of_extra_arguments = ['projectSourceDir', 'projectBinaryDir']
extra_arguments = {
	'projectSourceDir' : os.path.dirname(this_script_path),
	'projectBinaryDir' : project_binary_dir
	}
last_checked_argument = -1
for extra_argument in order_of_extra_arguments:
	if extra_argument in function_arg_spec:
		if function_arg_spec.index(extra_argument) < last_checked_argument:
			raise Exception ('Order of special arguments is wrong in the function signature')
		else:
			last_checked_argument = function_arg_spec.index(extra_argument)
		pass_args.append( extra_arguments[extra_argument] )

if inspect.getargspec( matched_command[1] ).varargs is None:
        # with varargs, such protection obviously makes no sense
        if len(real_args) > len(function_arg_spec):
            raise Exception('Error: Too many arguments for the chosen command')

pass_args.extend(args[len(matched_command[0]):])

try:
        matched_command[1]( *pass_args )  # pack arguments after the last chunk of the command
except Exception as e:
                print 'Failed because: '+str(e)+'.\nHint: look at the lines above, answer might be there.'
                traceback.print_exc()
                sys.exit(1)
sys.exit(0)
