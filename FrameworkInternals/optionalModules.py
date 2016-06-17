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
from shutil import copy, rmtree

moduleInfo = {}


def getEnabledModules():
	"""Get all enabled module metadata"""
	baseDirectory = os.getcwd()
	os.chdir(baseDirectory + os.path.sep + "FrameworkInternals")
	if os.path.exists("EnabledModules"): os.chdir("EnabledModules")
	else:
		print "No enabled modules."
		return
	moduleUrls = glob("*.url")
	enabledModules = {}
	for moduleUrl in moduleUrls:
		module = moduleUrl.replace(".url","")
		minVersion = None
		tag = None
		try:
			minVersion = open(module+".minVersion").readline().rstrip()
			tag = open(module+".tag").readline().rstrip()
		except Exception, ex:
			print ex
		if not minVersion:
			print "Error reading min version info for module "+module
			return None
		if not tag:
			print "Error reading tag info for module "+module
			return None
		enabledModules[module] = {"tag":tag, "minVersion":minVersion}
	os.chdir(baseDirectory)
	return enabledModules

def listEnabledModules():
	"""List registered module URLs"""
	enabledModules = getEnabledModules()
	print "Enabled optional modules and their required quasar versions: "
	for module in enabledModules.keys():
		print module, enabledModules[module]["tag"], "(requires quasar", enabledModules[module]["minVersion"]+")"

def getModuleInfo():
	"""Downloads list of modules from git and initializes global module list."""	

	baseDirectory = os.getcwd()
	os.chdir(baseDirectory + os.path.sep + "FrameworkInternals")
	#print("Changing directory to: " + baseDirectory + os.path.sep + "FrameworkInternals")
	if not os.path.exists("quasar-modules"): os.mkdir("quasar-modules")
	os.chdir("quasar-modules")
	#print("Changing directory to: quasar-modules")
	print("Checking out from git")
	if os.path.exists(".git"):
		try:
			subprocess.call("git pull origin master" , shell=True)
		except Exception, ex:
			print "Error trying to fetch optional module list from github:", ex
			return False
	else:
		try:
			subprocess.call("git init" , shell=True)
			subprocess.call("git remote add origin https://github.com/quasar-team/quasar-modules.git" , shell=True)
			subprocess.call("git remote set-url --push origin push-disabled" , shell=True)
			subprocess.call("git pull origin master" , shell=True)
		except Exception, ex:
			print "Error trying to fetch optional module list from github:", ex
			return False

	moduleUrls = glob("*.url")
	#print moduleUrls
	for moduleUrl in moduleUrls:
		module = moduleUrl.replace(".url","")
		minVersion = None
		try:
			minVersion = open(module+".minVersion").readline().rstrip()
		except Exception, ex:
			print ex
		if not minVersion:
			print "Error reading version info for module "+module
			return False
		moduleInfo[module] = minVersion
	print "List of existing optional modules and their required quasar versions: ", moduleInfo
	os.chdir(baseDirectory)
	return True

def enableModule(moduleName, tag="master"):
	"""Enables optional module. Module URL and required quasar version is downloaded from github. Module download is done later at cmake configure stage. If tag argument exists, use it, otherwise use master head.
	
	Keyword arguments:
	moduleName -- name of the optional module
	tag -- tag to checkout
	"""	
	print "Enabling module", moduleName, ", tag", tag

	if not getModuleInfo(): return False

	print "Checking module to be compatible..."
	quasarVersion = None
	try:
		quasarVersion = open("Design/quasarVersion.txt").readline().rstrip()
	except Exception, ex:
		print ex
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

	baseDirectory = os.getcwd()
	os.chdir(baseDirectory + os.path.sep + "FrameworkInternals")
	if not os.path.isdir("EnabledModules"): os.mkdir("EnabledModules")
	try:
		for file in glob("quasar-modules/"+moduleName+".*"):
			copy(file, "EnabledModules/")
		# add tag
		tagFileName = "EnabledModules/"+moduleName+".tag"
		if os.path.exists(tagFileName): os.remove(tagFileName)
		file = open(tagFileName, "w")
		file.write(tag)
	except Exception, ex:
		print "Failed to set up module files in FrameworkInternals/EnabledModules/ :", ex
		return False
	os.chdir(baseDirectory)

	print("Created module files.")

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
		for file in glob("FrameworkInternals/EnabledModules/"+moduleName+".*"):
			os.remove(file)
	except Exception, ex:
		print "Failed to remove module file in FrameworkInternals/EnabledModules/ :", ex
		return False

	print("Removed url file of "+moduleName)
	print("Remove module code if existing...")
	cleanModule(moduleName)

	return True

def cleanModule(module):
	dirs = glob(module+"*")
	if dirs:
		print "Removing files of module", module
		for dir in glob(module+"*"):
			print "Removing", dir
			try:
				rmtree(dir)
			except Exception, ex:
				print "Failed to remove dir", dir, ex
	else: print "Nothing to be removed for module", module

def cleanModules():
	"""Remove all enabled modules"""
	enabledModules = getEnabledModules()
	print "Removing downloaded modules"
	for module in enabledModules.keys():
		cleanModule(module)
