#!/usr/bin/env python
# encoding: utf-8
'''
distclean.py

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
import sys
import shutil
import platform
from commandMap import getCommand
from externalToolCheck import subprocessWithImprovedErrors

def deleteFolderRecursively( topdir, target ):
	for dirpath, dirnames, files in os.walk(topdir):
		for name in dirnames:
			if name == target:
				try:
					shutil.rmtree(os.path.join(dirpath, name))
				except OSError:
					pass
	return;

def deleteFileRecursively( topdir, target ):
	for dirpath, dirnames, files in os.walk(topdir):
		for name in files:
			if name == target:
				try:
					os.remove(os.path.join(dirpath, name))					
				except OSError:
					pass
	return;
	
def deleteExtensionRecursively( topdir, target ):
	for dirpath, dirnames, files in os.walk(topdir):
		for name in files:
			if name.endswith(target):
				try:
					os.remove(os.path.join(dirpath, name))					
				except OSError:
					pass
	return;
	
def distClean(*args, **kwargs):
	if len(args)>0:
		param = args[0]
	"""
	Cleaning method. It first calls msbuild/make clean and after that it will delete the leftover generated files in the Server.
	If parameter --orig is specified, the generated files ending with the extension .orig will also be cleared.	
	"""
	if platform.system() == "Windows":
		print('Calling visual studio vcvarsall to set the environment')
		print(getCommand("vcvarsall") + ' amd64')
		subprocessWithImprovedErrors("\"" + getCommand("vcvarsall") + '\" amd64', "visual studio vcvarsall.bat")

		print('Calling msbuild clean')
		print('msbuild ALL_BUILD.vcxproj /t:Clean')
		subprocessWithImprovedErrors("\"" + getCommand("vcvarsall") + '\" amd64 && ' + getCommand("msbuild") + ' ALL_BUILD.vcxproj /t:Clean', "visual studio msbuild", [0, 1])
	elif platform.system() == "Linux":
		subprocessWithImprovedErrors([getCommand('make'), 'clean'], getCommand("make"))	
	print('Deleting generated files and directories')
	deleteFolderRecursively('.', 'CMakeFiles')
	deleteFolderRecursively('.', 'Win32')
	deleteFolderRecursively('.', 'Debug')
	deleteFolderRecursively('.', 'Release')
	deleteFolderRecursively('.', 'x64')
	deleteFolderRecursively('.', '*.dir')
	deleteExtensionRecursively('.', 'vcxproj')
	deleteExtensionRecursively('.', 'sln')
	deleteExtensionRecursively('.', 'vcxproj.filters')
	deleteExtensionRecursively('.', 'vcxproj.user')
	deleteExtensionRecursively('.', 'sdf')
	deleteExtensionRecursively('.', 'opensdf')
	deleteFileRecursively('.', 'CMakeCache.txt')
	deleteFileRecursively('.', 'Makefile')
	deleteFileRecursively('.', 'cmake_install.cmake')
	deleteFileRecursively('.', 'cmake_generated.cmake')
	deleteFileRecursively('.', 'generated_files_list.cmake')
	deleteExtensionRecursively('.', 'pyc')
	if param == '--orig':
		print('Removing .orig files')
		deleteExtensionRecursively('.', 'orig')
