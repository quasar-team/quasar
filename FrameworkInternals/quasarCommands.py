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

from generateCmake import generateCmake
from automated_build import automatedBuild
from automated_build import build_config, set_build_config
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
from externalToolCheck import checkExternalDependencies
from optionalModules import enableModule, disableModule, listModules, listEnabledModules, removeModules, removeModule

# format is: [command name], callable, for_users
# where for_users is True for non-internal commands

commands = [
	[['generate','cmake_headers'], generateCmake, False],   # This one takes variable number of params
	[['prepare_build'], generateCmake, True],   #
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
	[['external_tool_check'], checkExternalDependencies, True],	
	[['enable_module'], enableModule, True],
	[['disable_module'], disableModule, True],
	[['list_modules'], listModules, True],
	[['list_enabled_modules'], listEnabledModules, True],
    [['build_config'], build_config, True],
    [['set_build_config'], set_build_config, True]
	]
	
def printCommandList():
	for cmd in commands:
		if cmd[2]:
			print ' '.join(cmd[0])
def getCommands():
	return commands

def getCommandFromFunction(function):
        matching = filter(lambda x: x[1] == function, commands)
        if len(matching) != 1:
                return ''
        return ' '.join(matching[0][0])

def extract_argument(input, key):
	""" If key is present in input, will remove it and the following element from the list. 
		Returns a tuple of the output list (i.e. after removal of two elements) and the value of the element (i.e. the second of the two) """ 
	if key in input:
		pos = input.index(key)
		output = input
		output.pop(pos)
                try:
		        value = output.pop(pos)
                except IndexError:
                        raise Exception ('Argument {0} requires to be followed by a value. Run out of arguments.'.format(key))
		return (output, value)
	else:  # nothing to do, argument not present
		return (input, None)

def extract_common_arguments(input):
	""" Will parse some common arguments and remove them from the input list """
	(input, project_binary_dir) = extract_argument(input, '--project_binary_dir')
        return (input, project_binary_dir)
