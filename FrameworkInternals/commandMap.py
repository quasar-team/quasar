
commandMap = {
"saxon":"saxon9he.jar",
"java":"java",
"astyle":"astyle",
"diff":"kdiff3",
"cmake":"cmake",
"make":"make",
"gcc":"gcc",
"xmllint":"xmllint",
"graphviz":"dot",
"doxygen":"doxygen",
"git":"git",
"indent":"indent"
}

def getCommand(key):
	"""
	Checks the command map, and returns the command specified by the key
	"""
	return commandMap[key]
