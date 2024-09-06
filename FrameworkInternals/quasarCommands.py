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
from manage_files import mfSetupSvnIgnore
from manage_files import symlinkRuntimeDeps
import manage_files
from deviceGenerators import generateDeviceClass, generateAllDevices
from configurationGenerators import generateConfiguration
from designTools import validateDesign
from designTools import formatDesign
from designTools import upgradeDesign
from designTools import createDiagram
from runDoxygen import runDoxygen
from externalToolCheck import checkExternalDependencies
from optionalModules import enableModule, disableModule, listModules, listEnabledModules, removeModules, removeModule
from transformDesign import transformByKey, TransformKeys
from quasar_basic_utils import extract_argument
import quasar_basic_utils

# format is: [command name], callable, for_users
# where for_users is True for non-internal commands

commands = [
        [['generate','cmake_headers'],    generateCmake, False],   # This one takes variable number of params
        [['prepare_build'],               generateCmake, True],   #
        [['generate','root'],             lambda context: transformByKey([TransformKeys.D_ROOT_H, TransformKeys.D_ROOT_CPP], {'context':context}), False],		 # This takes none - check
        [['generate','base_h'],           lambda context, className: transformByKey([TransformKeys.D_BASE_H], {'context':context, 'className':className}), False],
        [['generate','base_cpp_all'],     lambda context: transformByKey([TransformKeys.D_BASE_CPP_ALL], {'context':context}), False],
        [['generate','device','--all'],   generateAllDevices, True],
        [['generate','device'],           generateDeviceClass, True],
        [['generate','source_variables'], lambda context: transformByKey([TransformKeys.AS_SOURCEVARIABLES_H, TransformKeys.AS_SOURCEVARIABLES_CPP], {'context':context}), False],
        [['generate','info_model'],       lambda context: transformByKey([TransformKeys.AS_INFOMODEL_H, TransformKeys.AS_INFOMODEL_CPP], {'context':context}), False],
        [['generate','asclass'],          lambda context, className: transformByKey([TransformKeys.AS_CLASS_H], {'context':context, 'className':className}), False],
        [['generate','asclass_cpp_all'],  lambda context: transformByKey([TransformKeys.AS_CLASS_CPP_ALL], {'context':context}), False],
        [['generate','config_xsd'],       generateConfiguration, False],
        [['generate','config_cpp'],       lambda context: transformByKey(TransformKeys.CONFIGURATOR, {'context':context}), False],
        [['generate','config_validator'], lambda context: transformByKey(TransformKeys.CONFIG_VALIDATOR, {'context':context}), False],
        [['generate','config_doc'],       lambda context: transformByKey(TransformKeys.CONFIG_DOCUMENTATION, {'context':context}), True],
        [['generate','as_doc'],           lambda context: transformByKey(TransformKeys.AS_DOCUMENTATION,  {'context':context}), True],
        [['generate','config_doc_md'],    lambda context: transformByKey(TransformKeys.CONFIG_DOCUMENTATION_MD, {'context':context}), True],
        [['generate','as_doc_md'],        lambda context: transformByKey(TransformKeys.AS_DOCUMENTATION_MD,     {'context':context}), True],
        [['generate','diagram'],          createDiagram, True],
        [['check_consistency'], mfCheckConsistency, True],
        [['setup_svn_ignore'], mfSetupSvnIgnore, True],
        [['build'], automatedBuild, True],
        [['clean'], distClean, True],
        [['create_project'], createProject, True],
        [['create_release'], mfCreateRelease, False],
        [['upgrade_project'], upgradeProject, True],
        [['upgrade_design'], upgradeDesign, True],
        [['format_design'], formatDesign, True],
        [['validate_design'], validateDesign, True],
        [['doxygen'], runDoxygen, True],
        [['external_tool_check'], checkExternalDependencies, True],
        # commands for optional modules
        [['enable_module'], enableModule, True],
        [['disable_module'], disableModule, True],
        [['list_modules'], listModules, True],
        [['list_enabled_modules'], listEnabledModules, True],
        # commands for build config(s)
        [['build_config'], build_config, True],
        [['set_build_config'], set_build_config, True],
        # various commands for developers
        [['symlink_runtime_deps'],       symlinkRuntimeDeps, True],
        [['quasar_version'],              quasar_basic_utils.print_quasar_version, True],
        [['style_it'],                    manage_files.command_style_it, True]
        ]

def printCommandList():
    for cmd in commands:
        if cmd[2]:
            print(' '.join(cmd[0]))
def getCommands():
    return commands

def getCommandFromFunction(function):
    matching = [x for x in commands if x[1] == function]
    if len(matching) != 1:
        return ''
    return ' '.join(matching[0][0])

def extract_common_arguments(inData):
    """ Will parse some common arguments and remove them from the inData list """
    (inData, project_binary_dir) = extract_argument(inData, '--project_binary_dir')
    return (inData, project_binary_dir)
