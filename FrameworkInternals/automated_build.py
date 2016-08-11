#!/usr/bin/env python
# encoding: utf-8
'''
automated_build.py

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

@contact:    damian.abalo@cern.ch
'''

import os
import platform
import subprocess
from generateCmake import generateCmake
from externalToolCheck import subprocessWithImprovedErrors
from commandMap import getCommand

def findFileRecursively( topdir, target ):
	for dirpath, dirnames, files in os.walk(topdir):
		for name in files:
			if name == target:
				return True;
	return False;

def automatedBuild(buildType="Release",
		   cmakeToolchainFile="FrameworkInternals" + os.path.sep + "default_configuration.cmake"):
	"""Method that generates the cmake headers, and after that calls make/msbuild to compile your server.
	
	Keyword arguments:
	buildType -- Optional parameter to specify Debug or Release build. If it is not specified it will default to Release.
	"""	
	if buildType != "Release" and buildType != "Debug" and cmakeToolchainFile == "FrameworkInternals" + os.path.sep + "default_configuration.cmake":
		cmakeToolchainFile = buildType
		buildType = "Release"
	generateCmake(buildType, cmakeToolchainFile)			
			
	print('Calling make/msbuild')
	if platform.system() == "Windows":
		print('Calling visual studio vcvarsall to set the environment')
		vcvarsFullCommand = "\"" + getCommand("vcvarsall") + '\" amd64'
		print(vcvarsFullCommand)
		#First, vcvarsall is called. Python resets environment between subprocess calls, so this is not going to set up the variables as we need.
		#However we do this "fake" call to identify if there is any problem finding vcvarsall.bat
		subprocessWithImprovedErrors( vcvarsFullCommand, "visual studio vcvarsall.bat")
		
		msbuildFullCommand = getCommand("msbuild") + ' ALL_BUILD.vcxproj /clp:ErrorsOnly /property:Platform=x64;Configuration=' + buildType
		print(msbuildFullCommand)
		#After making sure the vcvarsall works, we concatenate the vcvarsall call with the msbuild call, so the environment is preserved between the calls.
		#Additionally, if there is any prolem on this call, we know it is coming from msbuild, since vcvarsall was previously tested.
		subprocessWithImprovedErrors( vcvarsFullCommand + ' && ' + msbuildFullCommand, "visual studio msbuild")
	elif platform.system() == "Linux":
		print('make -j$(nproc)')
		#we call process nproc and store its output
		process = subprocess.Popen(["nproc"], stdout=subprocess.PIPE)
		out, err = process.communicate()
		#this output is used for calling make
		subprocessWithImprovedErrors([getCommand("make"), "-j" + str(int(out))], getCommand("make"))#the conversion from string to int and back to string is to remove all whitespaces and ensure that we have an integer
