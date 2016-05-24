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

@contact:    damian.abalo@cern.ch
'''

import os
import os.path
import platform
import sys
import subprocess
from subprocess import Popen
import filecmp
import __main__
from commandMap import getCommand

VERBOSE = 1
XSLT_JAR = '.' + os.path.sep + 'Design' + os.path.sep + getCommand('saxon')

#def getVcvarsallPath(simpleQuotes=False):
#	"""
#	Method that returns the absolute path of vcvarsall.bat. Since this is hardcoded at the moment, and used trough different parts of the scripts, at least is hardcoded only once
#	It is returned in double quotation marks, because it needs to be used with quotes inside of the string, because of the spaces on the path
#	"""
#	if(simpleQuotes):
#		return "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat"
#	else:
#		return '"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat"'

def printIfVerbose(msg):
	if VERBOSE > 0:
		print(msg)

def checkJava():
	try:
		returnCode = subprocess.call([getCommand('java'), '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("Java does exist")
		else:
			raise Exception("Java cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nJava can be downloaded in https://www.java.com/en/download/ ")
	except:
		raise Exception("Java cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nJava can be downloaded in https://www.java.com/en/download/ ")
	
def checkSaxon():
	if os.path.isfile(XSLT_JAR):
		printIfVerbose("jaxon0he.jar does exist")
	else:
		raise Exception("jaxon0he.jar cannot be found in the Design folder. \njaxon0he.jar can be downloaded in http://saxon.sourceforge.net/#F9.7HE ")
	
def checkAstyle():
	try:
		returnCode = subprocess.call([getCommand('astyle'), '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("astyle does exist")
		else:
			raise Exception("Astyle cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nAstyle can be downloaded in http://astyle.sourceforge.net/ ")
	except:
		raise Exception("Astyle cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nAstyle can be downloaded in http://astyle.sourceforge.net/ ")
	
def checkKdiff3():
	try:
		returnCode = -1
		if platform.system() == "Windows":
			returnCode = subprocess.call(['where', getCommand('diff')], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		else:
			returnCode = subprocess.call([getCommand('diff'), '--help'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("kdiff3 does exist")
		else:
			raise Exception("kdiff3 cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nkdiff3 can be downloaded in http://kdiff3.sourceforge.net/ ")
	except:
		raise Exception("kdiff3 cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nkdiff3 can be downloaded in http://kdiff3.sourceforge.net/ ")

def checkCMake():
	try:
		returnCode = subprocess.call([getCommand('cmake'), '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("CMake does exist")
		else:
			raise Exception("CMake cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nCMake can be downloaded in https://cmake.org/ ")
	except:
		raise Exception("CMake cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nCMake can be downloaded in https://cmake.org/ ")
		
def checkCompiler():
	if platform.system() == "Windows":
		if os.path.isfile(getCommand('vcvarsall.simple')):
			printIfVerbose("vcvarsall.bat does exist")
		else:		
			raise Exception("vcvarsall.bat cannot be found in the default path [" + getCommand('vcvarsall.simple') + "]. Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
		try:
			returnCode = subprocess.call(getCommand('vcvarsall') + ' amd64 && ' + getCommand('msbuild') + ' /h', shell=True, stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
			if returnCode == 0:
				printIfVerbose("msbuild does exist")
			else:
				raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
		except:
			raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. ")
		
	elif platform.system() == "Linux":
		try:
			returnCode = subprocess.call([getCommand('make'), '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
			if returnCode == 0:
				printIfVerbose("make does exist")
			else:
				raise Exception("make cannot be found. Maybe it is not installed, or maybe it is not set in the PATH.")
		except:
			raise Exception("make cannot be found. Maybe it is not installed, or maybe it is not set in the PATH.")
			
def checkXMLLint():
	try:
		returnCode = subprocess.call([getCommand('xmllint'), '--version'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("XML Lint does exist")
		else:
			raise Exception("XML Lint cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nXML Lint can be downloaded in http://xmlsoft.org/ ")
	except:
		raise Exception("XML Lint cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nXML Lint can be downloaded in http://xmlsoft.org/ ")

#Non compulsory dependancy (Needed for generating graphs, but QUASAR will perfectly work without GraphViz)
def checkGraphViz():
	try:
		returnCode = subprocess.call([getCommand('graphviz'), '-V'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("GraphViz does exist")
		else:
			raise Exception("GraphViz cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nGraphViz can be downloaded in http://www.graphviz.org/ ")
	except:
		raise Exception("GraphViz cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nGraphViz can be downloaded in http://www.graphviz.org/ ")

#Non compulsory dependancy (Needed for generating documentation, but QUASAR will perfectly work without DoxyGen)
def checkDoxyGen():
	try:
		returnCode = -1
		if platform.system() == "Windows":
			returnCode = subprocess.call(['where', getCommand('doxygen')], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		else:
			returnCode = subprocess.call(['which', getCommand('doxygen')], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("DoxyGen does exist")
		else:
			raise Exception("DoxyGen cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nDoxyGen can be downloaded in http://www.stack.nl/~dimitri/doxygen/ ")
	except:
		raise Exception("DoxyGen cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nDoxyGen can be downloaded in http://www.stack.nl/~dimitri/doxygen/ ")
	
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
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py dependency_check")
	tryDependency(checkSaxon)
	tryDependency(checkJava)
	tryDependency(checkKdiff3)
	tryDependency(checkCMake)
	tryDependency(checkCompiler)
	tryDependency(checkXMLLint)
	tryDependency(checkAstyle, False)
	tryDependency(checkGraphViz, False)
	tryDependency(checkDoxyGen, False)
		
def subprocessWithImprovedErrors(subprocessCommand, dependencyName):
	"""Method that calls subprocess, but print intelligent error messages if there are any exceptions catched.
	
	Keyword arguments:
	subprocessCommand -- String or list of strings that will be given to subprocess
	dependencyName -- parameterless name of the command, just for error loging purposes
	"""	
	try:
		return subprocess.call(subprocessCommand)
	except OSError as e:		
		#print("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependancy is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py dependency_check'.")
		raise Exception("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependancy is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py dependency_check'.")
	except Exception, e:
		#print("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "].")
		raise Exception("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "]")

def subprocessWithImprovedErrorsPipeOutputToFile(subprocessCommand, outputFile, dependencyName):
	"""Method that calls subprocess, but print intelligent error messages if there are any exceptions catched, and then pipes the output of the process to the given file
	
	Keyword arguments:
	subprocessCommand -- String or list of strings that will be given to subprocess
	dependencyName -- parameterless name of the command, just for error loging purposes
	outputFile -- file where the std out of the process will be written into
	"""	
	try:
		with open(outputFile,"wb") as out:
			process = subprocess.Popen(subprocessCommand, stdout=out)
			streamdata = process.communicate()
			return process.returncode
	except OSError as e:		
		#print("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependancy is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py dependency_check'.")
		raise Exception("There was an OS error when trying to execute the program [" + dependencyName + "]. This probably means that a dependancy is missing or non-accesible; Exception: [" + str(e) + "]. For more details run the command 'quasar.py dependency_check'.")
	except Exception, e:
		#print("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "].")
		raise Exception("There was an application error when trying to execute the program [" + dependencyName + "]. Exception: [" + str(e) + "]")
