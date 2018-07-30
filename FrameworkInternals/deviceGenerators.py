#!/usr/bin/env python
# encoding: utf-8
'''
deviceGenerators.py

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

@contact:    quasar-developers@cern.ch
'''

import os
from transformDesign import TransformKeys, transformByKey
from manage_files import get_list_classes


def generateOneDeviceClass(className):
	transformByKey([TransformKeys.D_DEVICE_H, TransformKeys.D_DEVICE_CPP], {'className':className})
	
def generateDeviceClass(*classList):
	"""Generates the files D<classname>.h and D<classname>.cpp. This method needs to be called by the user, as this is the class where the device logic is, so a manual merge will be needed.
	
	Keyword arguments:
	classname -- the name of the device, which this class will be associated to. You can specify several classes (up to 10), separated by spaces or just --all to regenerate all device classes.
	"""
	if len(classList) < 1:
		raise Exception("BadArguments: need at least one arg for this") # TODO Another exception

	for aClass in classList:
		generateOneDeviceClass(aClass)
	
def generateAllDevices():
	"""Generates the files D<classname>.h and D<classname>.cpp for ALL the different devices. This method needs to be called by the user, as this is the class where the device logic is, so a manual merge will be needed.	"""
	project_directory = os.getcwd()		
	classes = get_list_classes(project_directory+os.path.sep+"Design"+os.path.sep+"Design.xml")  # TODO wrong - we should have a function returning it
	for aClass in classes:
		generateOneDeviceClass(aClass['name'])