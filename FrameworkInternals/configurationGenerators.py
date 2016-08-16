#!/usr/bin/env python
# encoding: utf-8
'''
configurationGenerators.py

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
import shutil
from transformDesign import transformDesignVerbose
from externalToolCheck import subprocessWithImprovedErrorsPipeOutputToFile
from commandMap import getCommand

configPath = "Configuration" + os.path.sep	
def generateConfiguration():
	"""Generates the file Configuration.xsd. This method is called automatically by cmake, it does not need to be called by the user."""
	outputFile = os.path.join('Configuration', 'Configuration.xsd')
	cleanedOutputFile = os.path.join('Configuration', 'Configuration.xsd.new')
	transformationFile = os.path.join(configPath, "designToConfigurationXSD.xslt")
	#output = "Configuration.xsd"
	transformDesignVerbose(transformationFile, outputFile, 0, 0)
	print("Calling xmllint to modify " + outputFile)
	#this call is not using subprocess with improved errors because of the need of the piping.
	subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--xinclude", outputFile], cleanedOutputFile, getCommand("xmllint"))
	print("Copying the modified file  " + cleanedOutputFile + " into the name of " + outputFile)
	shutil.copyfile(cleanedOutputFile, outputFile)

def generateConfigurator():
	"""Generates the file Configurator.cpp. This method is called automatically by cmake, it does not need to be called by the user."""
	output = "Configurator.cpp"
	transformDesignVerbose(configPath + "designToConfigurator.xslt", configPath + output, 0, 1)
	
def generateConfigValidator():
	"""Generates the file ConfigValidator.xsd. This method is called automatically by cmake, it does not need to be called by the user."""
	output = "ConfigValidator.cpp"
	transformDesignVerbose(configPath + "designToConfigValidator.xslt", configPath + output, 0, 1)			
