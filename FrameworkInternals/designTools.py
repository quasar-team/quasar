#!/usr/bin/env python
# encoding: utf-8
'''
designTools.py

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
import platform
import shutil
from transformDesign import transformDesignVerbose
from externalToolCheck import subprocessWithImprovedErrors
from externalToolCheck import subprocessWithImprovedErrorsPipeOutputToFile
from commandMap import getCommand

designPath = "Design" + os.path.sep
designXML = "Design.xml"
designXSD = "Design.xsd"

def validateDesign(projectBinaryDir):
	"""Checks design.xml against Design.xsd, and after that performs some additional checks (defined in designValidation.xslt)"""
	# 1st line of validation -- does it matches its schema?
	# This allows some basic checks
	print("1st line of check -- XSD conformance")
	print("Validating the file " + designXML + " with the schema " + designXSD)
	try:
		subprocessWithImprovedErrors([getCommand("xmllint"), "--noout", "--schema", designPath + designXSD, designPath + designXML], getCommand("xmllint"))
		# 2nd line of validation -- including XSLT
		print("2nd line of check -- more advanced checks using XSLT processor")
		output = "validationOutput.removeme"
		transformDesignVerbose(designPath + "designValidation.xslt", os.path.join(projectBinaryDir, "Design", output), 0, astyleRun=False)
	except Exception, e:
		raise Exception ("There was a problem validating the file [" + designXML + "]; Exception: [" + str(e) + "]")

def formatDesign():
	"""Formats design.xml. This is done to have always the same indentation format. The formatting is done in a separate file, in case something goes wrong, and then copied over."""
	backupName = designXML + ".backup"
	tempName = designXML + ".new"

	print("Creating a backup of " + designXML + " under the name of " + backupName)
	shutil.copyfile(designPath + designXML, designPath + backupName)

	print("Formatting the file " + designXML + "using the tool XMLlint. The result will be saved in " + tempName)
	try:
		if platform.system() == "Windows":
			subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), designPath + designXML], designPath + tempName, getCommand("xmllint"))
		elif platform.system() == "Linux":
			subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--format", designPath + designXML], designPath + tempName, getCommand("xmllint"))
	except Exception, e:
		raise Exception ("There was a problem formatting the file [" + designXML + "]; Exception: [" + str(e) + "]")
		
	print("Copying the formated file  " + tempName + " into the name of " + designXML)
	shutil.copyfile(designPath + tempName, designPath + designXML)
	
def upgradeDesign(additionalParam):
	"""Method for adjusting Design.xml for a new Design.xsd when updating to a new version of the Framework"""
	print("Formatting your design file ...")
	formatDesign()
	
	output = "Design.xml.upgraded"
	transformDesignVerbose(designPath + "designToUpgradedDesign.xslt", designPath + output, 0, astyleRun=False, additionalParam=additionalParam)
	
	print("Formatting the upgraded file ")
	formatedOutput = output + ".formatted"
	if platform.system() == "Windows":
		subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), designPath + output], designPath + formatedOutput, getCommand("xmllint"))
	elif platform.system() == "Linux":
		subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--format", designPath + output], designPath + formatedOutput, getCommand("xmllint"))
		
	print("Now running merge-tool. Please merge the upgraded changed")
	subprocessWithImprovedErrors([getCommand("diff"), "-o", designPath + designXML, designPath + designXML, designPath + formatedOutput], getCommand("diff"))
	
def createDiagram(detailLevel=0):
	"""Creates an UML diagram based on the classes of the server.
	
	Keyword arguments:
	detailLevel -- Detail level of the diagram. If it is not present, 0 will be assumed
	"""

	output = "Design.dot"
	transformDesignVerbose(designPath + "designToDot.xslt", designPath + output, 0, astyleRun=False, additionalParam="detailLevel=" + str(detailLevel))
	print("Generating pdf diagram with dot.")
	subprocessWithImprovedErrors([getCommand("graphviz"), "-Tpdf", "-o", designPath + "diagram.pdf", designPath + "Design.dot"], "GraphViz (dot)")
			
