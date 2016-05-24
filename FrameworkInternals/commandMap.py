
commandMap = {
"vcvarsall":"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat",
"java":"java",
"saxon":"saxon9he.jar",
"astyle":"astyle",
"diff":"kdiff3",
"cmake":"cmake",
"msbuild":"msbuild",
"make":"make",
"xmllint":"xmllint",
"graphviz":"dot",
"doxygen":"doxygen"
}

def getCommand(key):
	"""
	Checks the command map, and returns the command specified by the key
	"""
	return commandMap[key]