#!/usr/bin/env python
# encoding: utf-8
'''
optional_modules.py

@author:     Stefan Schlenker

@copyright:  2016 CERN

@license:
Copyright (c) 2016, CERN, Universidad de Oviedo.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    Stefan Schlenker
'''

import os
import subprocess
import platform
from glob import glob
from distutils.version import StrictVersion
from shutil import copy

moduleInfo = {}

def getModuleInfo():
	"""Downloads list of modules from git and initializes global module list."""	

	baseDirectory = os.getcwd()
	os.chdir(baseDirectory + os.path.sep + "FrameworkInternals")
	print("Changing directory to: " + baseDirectory + os.path.sep + "FrameworkInternals")
	if not os.path.exists("quasar-modules"): os.mkdir("quasar-modules")
	os.chdir("quasar-modules")
	print("Changing directory to: quasar-modules")
	print("Checking out from git")
	subprocess.call("git init;git remote add origin https://github.com/quasar-team/quasar-modules.git" , shell=True)
	subprocess.call("git pull origin master" , shell=True)

	moduleUrls = glob("*.url")
	print moduleUrls
	for moduleUrl in moduleUrls:
		module = moduleUrl.replace(".url","")
		minVersion = None
		try:
			minVersion = open(module+".minVersion").readline().rstrip()
		except exception:
			print exception
		if not minVersion: print "Error reading version info for module "+module
		moduleInfo[module] = minVersion
	print "moduleInfo", moduleInfo
	os.chdir(baseDirectory)
	print("Changing directory to: " + baseDirectory)

def enableModule(moduleName):
	"""Enables optional module. Module URL and required quasar version is downloaded from github. Module download is done later at cmake configure stage.
	
	Keyword arguments:
	moduleName -- name of the optional module
	"""	

	getModuleInfo()

	print "Checking module to be compatible..."
	quasarVersion = None
	try:
		quasarVersion = open("Design/quasarVersion.txt").readline().rstrip()
	except exception:
		print exception
	if not quasarVersion:
		print "Error reading version info from Design/quasarVersion.txt"
		return False
	moduleMinVersion = moduleInfo[moduleName]
	if StrictVersion(quasarVersion) >= StrictVersion(moduleMinVersion):
		print "Module "+moduleName+" required version "+moduleMinVersion+" is compatible with installed quasar version "+quasarVersion
	else:
		print "Cannot enable module "+moduleName+". Minimum required version "+moduleMinVersion+" is newer than installed quasar version "+quasarVersion
		return False

	print("Copying module url file...")

	if not os.path.isdir("FrameworkInternals/EnabledModules"): os.mkdir("FrameworkInternals/EnabledModules")
	try:
		copy("FrameworkInternals/quasar-modules/"+moduleName+".url", "FrameworkInternals/EnabledModules")
	except exception:
		print "Failed to set up module file in FrameworkInternals/EnabledModules/ :", exception
		return False

	print("Copied url file.")

	return True


def disableModule(moduleName):
	"""Disables optional module. Just disable the use of the module, no files will be deleted.
	
	Keyword arguments:
	moduleName -- name of the optional module
	"""	

	if not os.path.exists("FrameworkInternals/EnabledModules/"+moduleName+".url"):
		print "Error, module "+moduleName+" seems not installed!"
		return False
	try:
		os.remove("FrameworkInternals/EnabledModules/"+moduleName+".url")
	except exception:
		print "Failed to remove module file in FrameworkInternals/EnableModules/ :", exception
		return False

	print("Removed url file of "+moduleName)

	return True
