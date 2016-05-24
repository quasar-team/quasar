#!/usr/bin/env python
# encoding: utf-8
'''
install_framework.py

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

@contact:    damian.abalo@cern.ch
'''

import os
import platform
from manage_files import mfInstall
import __main__

def upgradeProject(destination):
	"""Upgrades the framework in a given directory
	
	Keyword arguments:
	destination -- The target directory where the framework will be installed or upgraded
	"""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py upgrade_project " + destination)
	if os.path.exists(destination) and os.path.exists(destination + os.path.sep + 'Device') and os.path.exists(destination + os.path.sep + 'Design') and os.path.exists(destination + os.path.sep + 'Configuration') and os.path.exists(destination + os.path.sep + 'AddressSpace'):
		destination = os.path.abspath(destination)
		print( "Selected installation folder: " + destination)
	else:
		print("Invalid path!")
		return -1		
	
	installFramework(destination)
	
	return 0
	
def createProject(destination):
	"""Installs the framework in a given directory. If the directory doesn't exist, it gets created
	
	Keyword arguments:
	destination -- The target directory where the framework will be installed or upgraded
	"""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py create_project " + destination)
	if os.path.exists(destination):
		destination = os.path.abspath(destination)
		print( "Selected installation folder: " + destination)
	else:
		destination = os.path.abspath(destination)
		os.makedirs(destination)
		print( "Created installation folder: " + destination)	
	installFramework(destination)
	
	return 0

def installFramework(destination):
	"""Installs or upgrades the framework in a given directory
	
	Keyword arguments:
	destination -- The target directory where the framework will be installed or upgraded
	"""	
		
	baseDirectory = os.getcwd()
	
	print("Calling mfInstall")
	returnCode = mfInstall(baseDirectory, destination)
	if returnCode != 0:
		print("There was a problem calling manage_files.py; Return code = " + str(returnCode))
		os.chdir(baseDirectory)
		return -1
	return 0