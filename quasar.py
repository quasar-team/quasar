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

@contact:    damian.abalo@cern.ch
'''

import os
import sys
import subprocess
internalFolders = ["AddressSpace", "Configuration", "Design", "Device", "FrameworkInternals", "Server", "LogIt", "Meta"]
initialDir = os.getcwd()
splittedPath = initialDir.split(os.path.sep)
splittedPathLength = len(splittedPath)
currentFolder = splittedPath[splittedPathLength - 1]
if(currentFolder in internalFolders):
	os.chdir("../")
sys.path.insert(0, './FrameworkInternals')

from generateCmake import generateCmake
from automated_build import automatedBuild
from distclean import distClean
from install_framework import createProject
from install_framework import upgradeProject
from manage_files import mfCheckConsistency
from manage_files import mfCreateRelease
from manage_files import mfInstall
from manage_files import mfSetupSvnIgnore
from manage_files import mfCheckSvnIgnore
from manage_files import mfDesignVsDevice
from deviceGenerators import generateRoot
from deviceGenerators import generateBaseClass
from deviceGenerators import generateDeviceClass
from addressSpaceGenerators import generateASClass
from addressSpaceGenerators import generateInformationModel
from addressSpaceGenerators import generateSourceVariables
from configurationGenerators import generateConfiguration
from configurationGenerators import generateConfigurator
from configurationGenerators import generateConfigValidator
from designTools import validateDesign
from designTools import formatDesign
from designTools import upgradeDesign
from designTools import createDiagram
from generateHonkyTonk import generateHonkyTonk
from runDoxygen import runDoxygen
from externalToolCheck import checkExternalDependancies

# format is: [command name], callable
commands = [
	[['generate','cmake_headers'], generateCmake, False],   # This one takes variable number of params
	[['generate','root'], generateRoot, False],		 # This takes none - check
	[['generate','base'], generateBaseClass, False],	 # 1 argument. Check that this works OK only for 1 arg
	[['generate','device'], generateDeviceClass, True],
	[['generate','source_variables'], generateSourceVariables, False],
	[['generate','info_model'], generateInformationModel, False],
	[['generate','asclass'], generateASClass, False],
	[['generate','config_xsd'], generateConfiguration, False],
	[['generate','config_cpp'], generateConfigurator, False],
	[['generate','config_validator'], generateConfigValidator, False],
	[['generate','honkytonk'], generateHonkyTonk, True],
	[['generate','diagram'], createDiagram, True],
	[['check_consistency'], mfCheckConsistency, True],
	[['setup_svn_ignore'], mfSetupSvnIgnore, True],
	[['build'], automatedBuild, True],
	[['clean'], distClean, True],
	[['create_project'], createProject, True],
	[['create_release'], mfCreateRelease, False],
	[['upgrade_project'], upgradeProject, True],
	[['design_vs_device'], mfDesignVsDevice, True],
	[['upgrade_design'], upgradeDesign, True],
	[['format_design'], formatDesign, True],
	[['validate_design'], validateDesign, True],	
	[['doxygen'], runDoxygen, True],	
	]


try:
	matched_command = filter(lambda x: x[0] == sys.argv[1:1+len(x[0])], commands)[0]
except IndexError:
	print 'Sorry, no such command. These are available:'
	for cmd in commands:
		if cmd[2]:
			print ' '.join(cmd[0])
	sys.exit(1)

if '-h' in sys.argv or '--help' in sys.argv:
	help(matched_command[1])
	sys.exit(0)
else:
	#check that all the external dependancies are working before starting the execution
	if matched_command[0] == ['generate','diagram']:
		checkExternalDependancies(True, False)
	elif matched_command[0] == ['doxygen']:
		checkExternalDependancies(False, True)
	else:
		checkExternalDependancies(False, False)
	exit_code = matched_command[1]( * sys.argv[1+len(matched_command[0]):])  # pack arguments after the last chunk of the command
	sys.exit(exit_code)
