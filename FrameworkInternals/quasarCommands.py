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
from externalToolCheck import checkExternalDependencies
from optionalModules import enableModule, disableModule, listModules, listEnabledModules, removeModules, removeModule

# format is: [command name], callable
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
