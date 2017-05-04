#!/usr/bin/env python
# encoding: utf-8
'''
externalToolCheck.py

@author:     Damian Abalo Miron <damian.abalo@cern.ch>

@copyright:  2016 CERN

@license:
Copyright (c) 2016, CERN, Universidad de Oviedo.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import os
import os.path
import platform
import sys
import subprocess
from subprocess import Popen
import filecmp
from commandMap import getCommand

VERBOSE = 1

def printIfVerbose(msg):
	if VERBOSE > 0:
		print(msg)

def checkExecutableExists(executableKeyName, doesNotExistErrorMessage, executableArgument='-h'):
	errorMessage = "executable [key:"+executableKeyName+", command: "+getCommand(executableKeyName)+"] cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \n"+doesNotExistErrorMessage
	try:
		returnCode = subprocess.call([getCommand(executableKeyName), executableArgument], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("executable [key:"+executableKeyName+", command: "+getCommand(executableKeyName)+"] exists")
		else:
			raise Exception(errorMessage)
	except:
		raise Exception(errorMessage)

def checkJava():
	checkExecutableExists('java', 'Java can be downloaded in https://www.java.com/en/download/')
	
def checkAstyle():
	checkExecutableExists('astyle', 'Astyle can be downloaded in http://astyle.sourceforge.net/')
	
def checkKdiff3():
		if platform.system() == "Linux":
			return checkExecutableExists('diff', 'kdiff3 can be downloaded in http://kdiff3.sourceforge.net/', '--help')
		#if the system is a windows machine then:
		errorMessageWindows = "kdiff3 cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nkdiff3 can be downloaded in http://kdiff3.sourceforge.net/ "
		try:
			returnCode = subprocess.call(['where', getCommand('diff')], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
			if returnCode == 0:
				printIfVerbose("kdiff3 does exist")
			else:
				raise Exception(errorMessageWindows)
		except:
			raise Exception(errorMessageWindows)

def checkCMake():
	checkExecutableExists('cmake', 'CMake can be downloaded in https://cmake.org/')
		
def checkCompiler():
	if platform.system() == "Linux":
		checkExecutableExists('make', 'Please, install the package make using the package manager of your distribution.')
		return checkExecutableExists('gcc', 'Please, install the package gcc using the package manager of your distribution.', '--help')
	#if the system is a windows machine then:
	if os.path.isfile(getCommand('vcvarsall')):
		printIfVerbose("vcvarsall.bat does exist")
	else:		
		raise Exception("vcvarsall.bat cannot be found in the default path [" + getCommand('vcvarsall') + "]. Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
	try:
		returnCode = subprocess.call("\"" + getCommand('vcvarsall') + "\"" + ' amd64 && ' + getCommand('msbuild') + ' /h', shell=True, stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("msbuild does exist")
		else:
			raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
	except:
		raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
		
			
def checkXMLLint():
	checkExecutableExists('xmllint', 'XML Lint can be downloaded in http://xmlsoft.org/', '--version')

#Non compulsory dependancy (Needed for generating graphs, but QUASAR will perfectly work without GraphViz)
def checkGraphViz():
	checkExecutableExists('graphviz', 'GraphViz can be downloaded in http://www.graphviz.org/', '-V')

#Non compulsory dependancy (Needed for generating documentation, but QUASAR will perfectly work without DoxyGen)
def checkDoxyGen():
	checkExecutableExists('doxygen', 'DoxyGen can be downloaded in http://www.stack.nl/~dimitri/doxygen/', '--version')
	
def tryDependency(functionCheck, critical=True):
	try:
		functionCheck()
	except Exception, e:
		if(critical):
			print("CRITICAL dependency missing: " + str(e))
		else:
			print("Optional dependency missing: " + str(e))

def checkExternalDependencies():
	"""Checks all of QUASAR dependencies to see if everything is setup as expected, and prints apropiate messages to point out what is missing."""
	tryDependency(checkJava)
	tryDependency(checkKdiff3)
	tryDependency(checkCMake)
	tryDependency(checkCompiler)
	tryDependency(checkXMLLint)
	tryDependency(checkAstyle)
	tryDependency(checkGraphViz, False)
	tryDependency(checkDoxyGen, False)
		
def subprocessWithImprovedErrors(subprocessCommand, dependencyName, validReturnCodes=[0]):
	"""Method that calls subprocess, but print intelligent error messages if there are any exceptions caught.
	
	Keyword arguments:
	subprocessCommand -- String or list of strings that will be given to subprocess
	dependencyName -- parameterless name of the command, just for error loging purposes
	validReturnCodes -- array of acceptable return codes, only 0 by default. If the return code is not in the list and exception will be thrown
	"""	
	try:
		returnCode = subprocess.call(subprocessCommand)
	except OSError as e:		
		raise Exception("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependency is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py external_tool_check'.")
	except Exception, e:
		raise Exception("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "]")
	if(returnCode not in validReturnCodes):
		raise Exception("Application returned bad return code when trying to execute the program [" + dependencyName + "]. Return code: [" + str(returnCode) + "]")

def subprocessWithImprovedErrorsPipeOutputToFile(subprocessCommand, outputFile, dependencyName, validReturnCodes=[0]):
	"""Method that calls subprocess, but print intelligent error messages if there are any exceptions catched, and then pipes the output of the process to the given file
	
	Keyword arguments:
	subprocessCommand -- String or list of strings that will be given to subprocess
	dependencyName -- parameterless name of the command, just for error loging purposes
	outputFile -- file where the std out of the process will be written into
	validReturnCodes -- array of acceptable return codes, only 0 by default. If the return code is not in the list and exception will be thrown
	"""	
	try:
		with open(outputFile,"wb") as out:
			process = subprocess.Popen(subprocessCommand, stdout=out)
			streamdata = process.communicate()
			returnCode = process.returncode
	except OSError as e:		
		raise Exception("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependency is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py external_tool_check'.")
	except Exception, e:
		raise Exception("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "]")
	if(returnCode not in validReturnCodes):
		raise Exception("Application returned bad return code when trying to execute the program [" + dependencyName + "]. Return code: [" + str(returnCode) + "]")
