from generateCmake import generateCmake
from automated_build import automatedBuild
from automated_build import build_config, set_build_config
from distclean import distClean
from install_framework import createProject
from install_framework import upgradeProject
from manage_files import mfCheckConsistency
from manage_files import mfCreateRelease
from manage_files import mfSetupSvnIgnore
from manage_files import mfDesignVsDevice
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

# format is: [command name], callable
commands = [
	[['generate','cmake_headers'],    generateCmake, False],   # This one takes variable number of params
	[['prepare_build'],               generateCmake, True],   #
	[['generate','root'],             lambda context: transformByKey([TransformKeys.D_ROOT_H, TransformKeys.D_ROOT_CPP], {'context':context}), False],		 # This takes none - check
	[['generate','base'],             lambda context, className: transformByKey([TransformKeys.D_BASE_H, TransformKeys.D_BASE_CPP], {'context':context, 'className':className}), False],
	[['generate','device','--all'],   generateAllDevices, True],
	[['generate','device'],           generateDeviceClass, True],
	[['generate','source_variables'], lambda context: transformByKey([TransformKeys.AS_SOURCEVARIABLES_H, TransformKeys.AS_SOURCEVARIABLES_CPP], {'context':context}), False],
	[['generate','info_model'],       lambda context: transformByKey([TransformKeys.AS_INFOMODEL_H, TransformKeys.AS_INFOMODEL_CPP], {'context':context}), False],
	[['generate','asclass'],          lambda context, className: transformByKey([TransformKeys.AS_CLASS_H, TransformKeys.AS_CLASS_CPP], {'context':context, 'className':className}), False],
	[['generate','config_xsd'],       generateConfiguration, False],
	[['generate','config_cpp'],       lambda context: transformByKey(TransformKeys.CONFIGURATOR, {'context':context}), False],
	[['generate','config_validator'], lambda context: transformByKey(TransformKeys.CONFIG_VALIDATOR, {'context':context}), False],
	[['generate','honkytonk'],        lambda context: transformByKey(TransformKeys.HONKYTONK, {'context':context}), True],
	[['generate','diagram'],          createDiagram, True],
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

def extract_argument(inData, key):
	""" If key is present in inData, will remove it and the following element from the list. 
		Returns a tuple of the output list (i.e. after removal of two elements) and the value of the element (i.e. the second of the two) """ 
	if key in inData:
		pos = inData.index(key)
		output = inData
		output.pop(pos)
		try:
			value = output.pop(pos)
		except IndexError:
			raise Exception ('Argument {0} requires to be followed by a value. Run out of arguments.'.format(key))
		return (output, value)
	else:  # nothing to do, argument not present
		return (inData, None)

def extract_common_arguments(inData):
	""" Will parse some common arguments and remove them from the inData list """
	(inData, project_binary_dir) = extract_argument(inData, '--project_binary_dir')
	return (inData, project_binary_dir)
