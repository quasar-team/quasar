#!/usr/bin/env python
# encoding: utf-8
'''
transformDesign.py

@author:     Piotr Nikiel <piotr@nikiel.info>
@author:     Damian Abalo Miron <damian.abalo@cern.ch>

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
import os.path
import sys
import filecmp
from externalToolCheck import subprocessWithImprovedErrors
from commandMap import getCommand

# args:
# $1    xslt transform filename
# $2	output filename
# $3	if 1, will prevent from overwriting output filename, running merge-tool
# $4	if 1, will run astyle on generated file
# $5	additional params to the transform

def transformDesignVerbose(xsltTransformation, outputFile, overwriteProtection, astyleRun, additionalParam=None):
	"""Prints information and then calls transformDesign
	
	Keyword arguments:
	xsltTransformation -- xslt file where the transformation is defined
	outputFile -- name of the file to be generated
	overwriteProtection -- if 1, will prevent from overwriting output filename, running merge-tool
	astyleRun -- if 1, will run astyle on generated file
	additionalParam -- Optional extra param. If specified, it will be given directly to saxon9he.jar
	"""
	if additionalParam == None:
		print("Using the transform [" + xsltTransformation + "] to generate the file [" + outputFile + "]")
	else:
		print("Using the transform [" + xsltTransformation + "] to generate the file [" + outputFile + "]; Additional param = [" + additionalParam + "]")
	return transformDesign(xsltTransformation, outputFile, overwriteProtection, astyleRun, additionalParam)

def transformDesign(xsltTransformation, outputFile, overwriteProtection, astyleRun, additionalParam=None):
	"""Generates a file, applying an xslt transform to Design.xml This is done calling saxon9he.jar
	
	Keyword arguments:
	xsltTransformation -- xslt file where the transformation is defined
	outputFile -- name of the file to be generated
	overwriteProtection -- if 1, will prevent from overwriting output filename, running merge-tool
	astyleRun -- if 1, will run astyle on generated file
	additionalParam -- Optional extra param. If specified, it will be given directly to saxon9he.jar
	"""
	#files
	XSLT_JAR = '.' + os.path.sep + 'Design' + os.path.sep + getCommand('saxon')
	DESIGN_XML = '.' + os.path.sep + 'Design' + os.path.sep + 'Design.xml'
	if overwriteProtection == 1:
		originalOutputFile = outputFile
		outputFile = outputFile + '.generated'
	try:
		#Do transformation
		if additionalParam == None:		
			subprocessWithImprovedErrors([getCommand('java'), '-jar', XSLT_JAR, DESIGN_XML, xsltTransformation, '-o:' + outputFile], getCommand("java"))		
		else:
			subprocessWithImprovedErrors([getCommand('java'), '-jar', XSLT_JAR, DESIGN_XML, xsltTransformation, '-o:' + outputFile, additionalParam], getCommand("java"))	

		if astyleRun == 1:		
			subprocessWithImprovedErrors([getCommand('astyle'), outputFile], getCommand("astyle"))

		if overwriteProtection == 1:
			#If the file existed previously and it is different from the old one we run kdiff3
			if (os.path.isfile(originalOutputFile)) and (filecmp.cmp(originalOutputFile, outputFile) == False):
				subprocessWithImprovedErrors([getCommand('diff'), "-o", originalOutputFile, originalOutputFile, outputFile], getCommand("diff"), [0, 1])#1 is a valid return, since it means that the user quitted without saving the merge, and this is still ok.
			else:#If the file didn't exist before, or it is equal to the old one, we rename the generated file to have the proper name
				if os.path.isfile(originalOutputFile):
					os.remove(originalOutputFile)
				os.rename(outputFile, originalOutputFile)
	except Exception, e:
		raise Exception ("There was a problem generating file [" + outputFile + "]; Exception: [" + str(e) + "]")