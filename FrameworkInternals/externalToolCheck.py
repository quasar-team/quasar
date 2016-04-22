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
import filecmp
import time

VERBOSE = 0
XSLT_JAR = '.' + os.path.sep + 'Design' + os.path.sep + 'saxon9he.jar'
VCVARSALL = "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat"
def printIfVerbose(msg):
	if VERBOSE > 0:
		print(msg)

def checkJava():
	try:
		returnCode = subprocess.call(['java', '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("Java does exist")
		else:
			raise Exception("Java cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nJava can be downloaded in https://www.java.com/en/download/ \nThe script will stop now.")
	except:
		raise Exception("Java cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nJava can be downloaded in https://www.java.com/en/download/ \nThe script will stop now.")
	
def checkSaxon():
	if os.path.isfile(XSLT_JAR):
		printIfVerbose("jaxon0he.jar does exist")
	else:
		raise Exception("jaxon0he.jar cannot be found in the Design folder. \njaxon0he.jar can be downloaded in http://saxon.sourceforge.net/#F9.7HE \nThe script will stop now.")
	
def checkAstyle():
	try:
		returnCode = subprocess.call(['astyle', '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("astyle does exist")
		else:
			raise Exception("Astyle cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nAstyle can be downloaded in http://astyle.sourceforge.net/ \nThe script will stop now.")
	except:
		raise Exception("Astyle cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nAstyle can be downloaded in http://astyle.sourceforge.net/ \nThe script will stop now.")
	
def checkKdiff3():
	try:
		returnCode = -1
		if platform.system() == "Windows":
			returnCode = subprocess.call(['where', 'kdiff3'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		else:
			returnCode = subprocess.call(['kdiff3', '--help'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("kdiff3 does exist")
		else:
			raise Exception("kdiff3 cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nkdiff3 can be downloaded in http://kdiff3.sourceforge.net/ \nThe script will stop now.")
	except:
		raise Exception("kdiff3 cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nkdiff3 can be downloaded in http://kdiff3.sourceforge.net/ \nThe script will stop now.")

def checkCMake():
	try:
		returnCode = subprocess.call(['cmake', '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("CMake does exist")
		else:
			raise Exception("CMake cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nCMake can be downloaded in https://cmake.org/ \nThe script will stop now.")
	except:
		raise Exception("CMake cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nCMake can be downloaded in https://cmake.org/ \nThe script will stop now.")
		
def checkCompiler():
	if platform.system() == "Windows":
		if os.path.isfile(VCVARSALL):
			printIfVerbose("vcvarsall.bat does exist")
		else:		
			raise Exception("vcvarsall.bat cannot be found in the default path [" + VCVARSALL + "]. Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. \nThe script will stop now.")
		try:
			returnCode = subprocess.call('"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat" amd64 && msbuild /h', shell=True, stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
			if returnCode == 0:
				printIfVerbose("msbuild does exist")
			else:
				raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. \nThe script will stop now.")
		except:
			raise Exception("msbuild cannot be properly executed after calling vcvarsall.bat . Maybe Visual Studio 2013 is not installed, or there is a problem with your installation. \nThe script will stop now.")
		
	elif platform.system() == "Linux":
		try:
			returnCode = subprocess.call(['make', '-h'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
			if returnCode == 0:
				printIfVerbose("make does exist")
			else:
				raise Exception("make cannot be found. Maybe it is not installed, or maybe it is not set in the PATH.\nThe script will stop now.")
		except:
			raise Exception("make cannot be found. Maybe it is not installed, or maybe it is not set in the PATH.\nThe script will stop now.")
			
def checkXMLLint():
	try:
		returnCode = subprocess.call(['xmllint', '--version'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("XML Lint does exist")
		else:
			raise Exception("XML Lint cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nXML Lint can be downloaded in http://xmlsoft.org/ \nThe script will stop now.")
	except:
		raise Exception("XML Lint cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nXML Lint can be downloaded in http://xmlsoft.org/ \nThe script will stop now.")

#Non compulsory dependancy (Needed for generating graphs, but QUASAR will perfectly work without GraphViz)
def checkGraphViz():
	try:
		returnCode = subprocess.call(['dot', '-V'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("GraphViz does exist")
		else:
			raise Exception("GraphViz cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nGraphViz can be downloaded in http://www.graphviz.org/ \nThe script will stop now.")
	except:
		raise Exception("GraphViz cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nGraphViz can be downloaded in http://www.graphviz.org/ \nThe script will stop now.")

#Non compulsory dependancy (Needed for generating documentation, but QUASAR will perfectly work without DoxyGen)
def checkDoxyGen():
	try:
		returnCode = -1
		if platform.system() == "Windows":
			returnCode = subprocess.call(['where', 'doxygen'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		else:
			returnCode = subprocess.call(['which', 'doxygen'], stdout=open(os.devnull, 'wb'), stderr=subprocess.STDOUT)
		if returnCode == 0:
			printIfVerbose("DoxyGen does exist")
		else:
			raise Exception("DoxyGen cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nDoxyGen can be downloaded in http://www.stack.nl/~dimitri/doxygen/ \nThe script will stop now.")
	except:
		raise Exception("DoxyGen cannot be found. Maybe it is not installed, or maybe it is not set in the PATH. \nDoxyGen can be downloaded in http://www.stack.nl/~dimitri/doxygen/ \nThe script will stop now.")
	
def checkExternalDependancies(usingGraphViz, usingDoxyGen):
	start_time = time.time()
	checkSaxon()
	checkJava()
	checkAstyle()
	checkKdiff3()
	checkCMake()
	checkCompiler()
	checkXMLLint()
	if usingGraphViz:
		checkGraphViz()
	if usingDoxyGen:
		checkDoxyGen()
	printIfVerbose('check completed in [' + str(time.time()-start_time) + '] seconds')