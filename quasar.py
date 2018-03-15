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

def extract_argument(input, key):
	""" If key is present in input, will remove it and the following element from the list. 
		Returns a tuple of the output list (i.e. after removal of two elements) and the value of the element (i.e. the second of the two) """ 
	if key in input:
		pos = input.index(key)
		output = input
		output.pop(pos)
		value = output.pop(pos)
		return (output, value)
	else:  # nothing to do, argument not present
		return (input, None)

def extract_common_arguments(input):
	""" Will parse some common arguments and remove them from the input list """
	(input, project_binary_dir) = extract_argument(input, '--project_binary_dir')
	return (input, project_binary_dir)

internalFolders = ["AddressSpace", "Configuration", "Design", "Device", "FrameworkInternals", "Server", "LogIt", "Meta"]
initialDir = os.getcwd()
splittedPath = initialDir.split(os.path.sep)
splittedPathLength = len(splittedPath)
currentFolder = splittedPath[splittedPathLength - 1]
if(currentFolder in internalFolders):
	os.chdir("../")
sys.path.insert(0, './FrameworkInternals')

from quasarCommands import printCommandList
from quasarCommands import getCommands

(args, project_binary_dir) = extract_common_arguments(sys.argv)
if project_binary_dir is None:
	project_binary_dir = os.getcwd()

if len(args) < 2:
        print 'The script was run without specifying what to do. Here are available commands:'
        printCommandList()
        sys.exit(1)

try:
	commands = getCommands()
	matched_command = filter(lambda x: x[0] == args[1:1+len(x[0])], commands)[0]
except IndexError:
	print 'Sorry, no such command. These are available:'
	printCommandList()
	sys.exit(1)

if '-h' in sys.argv or '--help' in sys.argv:
	help(matched_command[1])
	sys.exit(0)
else:
        try:
                args = sys.argv[1+len(matched_command[0]):]
                matched_command[1]( *args, projectBinaryDir=project_binary_dir )  # pack arguments after the last chunk of the command                
        except Exception as e:
                print 'Failed because: '+str(e)+'.\nHint: look at the lines above, answer might be there.'
                sys.exit(1)
	sys.exit(0)
