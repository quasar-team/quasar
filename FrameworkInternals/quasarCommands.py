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
from deviceGenerators import generateDeviceClass
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
	[['generate','cmake_headers'], generateCmake, False],   # This one takes variable number of params
	[['prepare_build'], generateCmake, True],   #
	[['generate','root'],             lambda: transformByKey([TransformKeys.D_ROOT_H, TransformKeys.D_ROOT_CPP]), False],		 # This takes none - check
	[['generate','base'],             lambda className: transformByKey([TransformKeys.D_BASE_H, TransformKeys.D_BASE_CPP], {'className':className}), False],	 # 1 argument. Check that this works OK only for 1 arg
	[['generate','device'], generateDeviceClass, True],
	[['generate','source_variables'], lambda: transformByKey([TransformKeys.AS_SOURCEVARIABLES_H, TransformKeys.AS_SOURCEVARIABLES_CPP]), False],
	[['generate','info_model'],       lambda: transformByKey([TransformKeys.AS_INFOMODEL_H, TransformKeys.AS_INFOMODEL_CPP]), False],
	[['generate','asclass'],          lambda className: transformByKey([TransformKeys.AS_CLASS_H, TransformKeys.AS_CLASS_CPP], {'className':className}), False],
	[['generate','config_xsd'],       generateConfiguration, False],
	[['generate','config_cpp'],       lambda: transformByKey(TransformKeys.CONFIGURATOR), False],
	[['generate','config_validator'], lambda: transformByKey(TransformKeys.CONFIG_VALIDATOR), False],
	[['generate','honkytonk'],        lambda: transformByKey(TransformKeys.HONKYTONK), True],
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
