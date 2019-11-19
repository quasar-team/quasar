#!/usr/bin/env python
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
import webbrowser

this_script_path = os.path.abspath(sys.argv[0])
sys.path.insert(0, os.path.join(os.path.dirname(this_script_path), 'FrameworkInternals'))

from quasarCommands import printCommandList
from quasarCommands import getCommands, extract_common_arguments
from quasarExceptions import WrongReturnValue, WrongArguments, Mistake

# args starts from the command name (e.g. 'build') and skips the common arguments (e.g. 'project_binary_dir')
(args, project_binary_dir) = extract_common_arguments(sys.argv[1:])  # 1: to skip the script name given by the operating system
if project_binary_dir is None:
    if ('build' in args or 'prepare_build' in args):
        print('Note: since quasar 1.3.0, by default, your build will be made in build/ directory.')
        print('If you need in-source build, try:  ./quasar.py build --project_binary_dir . ')
    project_binary_dir = os.path.join(os.getcwd(),'build')

def makeContext():
    """Generates a dictionary specifying the context.
    A context contains information necessary for build like:
    * projectSourceDirectory,
    * projectBinaryDirectory
    etc ... """
    context = {}
    context['projectSourceDir'] = os.path.dirname(this_script_path)
    context['projectBinaryDir'] = project_binary_dir
    return context

if len(args) < 1:
    print('The script was run without specifying what to do. Here are available commands:')
    printCommandList()
    sys.exit(1)

try:
    commands = getCommands()
    matched_command = [x for x in commands if x[0] == args[:len(x[0])]][0]
except IndexError:
    print('Sorry, no such command. These are available:')
    printCommandList()
    sys.exit(1)

if '-h' in args or '--help' in args:
    anchor = '_'.join(matched_command[0])
    print('Will open quasarCommands.html for anchor {0}'.format(anchor))
    webbrowser.open("file:///{htmlPath}#{anchor}".format(
            htmlPath=os.path.join(os.getcwd(),'Documentation','quasarCommands.html'),
            anchor=anchor))
    sys.exit(0)
else:
    try:  # we print exceptions from external tools, but internal ones we want to have with stack trace or PDB capability
        args = args[len(matched_command[0]):]
        callee = matched_command[1]
        # TODO throw WrongArguments here if not enough args
        if 'context' in inspect.getargspec(callee).args:
            callee( makeContext(), *args )  # pack arguments after the last chunk of the command
        else:
            callee( *args )
    except (WrongReturnValue, WrongArguments, Mistake) as e:
        print(str(e))
        sys.exit(1)
