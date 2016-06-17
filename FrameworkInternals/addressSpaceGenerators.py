#!/usr/bin/env python
# encoding: utf-8
'''
addressSpaceGenerators.py

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
from transformDesign import transformDesignVerbose
asPath = "AddressSpace" + os.path.sep
def generateSourceVariables():
	"""Generates the files SourceVariables.h and SourceVariables.cpp. This method is called automatically by cmake, it does not need to be called by the user."""
	output = "include/SourceVariables.h"
	transformDesignVerbose(asPath + "designToSourceVariablesHeader.xslt", asPath + output, 0, 1)
	output = "src/SourceVariables.cpp"
	transformDesignVerbose(asPath + "designToSourceVariablesBody.xslt", asPath + output,0, 1)

def generateASClass(classname):
	"""Generates the files AS<classname>.h and AS<classname>.cpp. This method is called automatically by cmake, it does not need to be called by the user.
	
	Keyword arguments:
	classname -- the name of the device, which this class will be associated to.
	"""
	output = "include/AS" + classname + ".h"
	transformDesignVerbose(asPath + "designToClassHeader.xslt", asPath + output, 0, 1, "className=" + classname)
		
	output = "src/AS" + classname + ".cpp"
	transformDesignVerbose(asPath + "designToClassBody.xslt", asPath + output, 0, 1,"className=" + classname)
	
def generateInformationModel():
	"""Generates the files ASInformationModel.h and ASInformationModel.cpp. This method is called automatically by cmake, it does not need to be called by the user."""
	output = "include/ASInformationModel.h"
	transformDesignVerbose(asPath + "designToInformationModelHeader.xslt", asPath + output, 0, 1)
	output = "src/ASInformationModel.cpp"
	transformDesignVerbose(asPath + "designToInformationModelBody.xslt", asPath + output,0, 1)
			