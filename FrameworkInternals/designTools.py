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

@contact:    damian.abalo@cern.ch
'''

import os
import platform
import shutil
import __main__
from transformDesign import transformDesignVerbose
from externalToolCheck import subprocessWithImprovedErrors
from externalToolCheck import subprocessWithImprovedErrorsPipeOutputToFile
from commandMap import getCommand

designPath = "Design" + os.path.sep
designXML = "Design.xml"
designXSD = "Design.xsd"

def validateDesign():
	"""Checks design.xml against Design.xsd, and after that performs some additional checks (defined in designValidation.xslt)"""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py validate_design")
	# 1st line of validation -- does it matches its schema?
	# This allows some basic checks
	print("1st line of check -- XSD conformance")
	print("Validating the file " + designXML + " with the schema " + designXSD)
	returnCode = subprocessWithImprovedErrors([getCommand("xmllint"), "--noout", "--schema", designPath + designXSD, designPath + designXML], getCommand("xmllint"))
	if returnCode != 0:
		print("There was a problem validating the file" + designXML + " with the schema " + designXSD + "; Return code = " + str(returnCode))
		return returnCode

	# 2nd line of validation -- including XSLT
	print("2nd line of check -- more advanced checks using XSLT processor")
	output = "validationOutput.removeme"
	returnCode = transformDesignVerbose(designPath + "designValidation.xslt", designPath + output, 0, 0)
	return 0

def formatDesign():
	"""Formats design.xml. This is done to have always the same indentation format. The formatting is done in a separate file, in case something goes wrong, and then copied over."""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py format_design")
	backupName = designXML + ".backup"
	tempName = designXML + ".new"

	print("Creating a backup of " + designXML + " under the name of " + backupName)
	shutil.copyfile(designPath + designXML, designPath + backupName)

	print("Formatting the file " + designXML + "using the tool XMLlint. The result will be saved in " + tempName)
	if platform.system() == "Windows":
		returnCode = subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), designPath + designXML], designPath + tempName, getCommand("xmllint"))
	elif platform.system() == "Linux":
		returnCode = subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--format", designPath + designXML], designPath + tempName, getCommand("xmllint"))
	if returnCode != 0:
		print("There was a problem Formatting the file " + designXML + "; Return code = " + str(returnCode))
		return returnCode
		
	print("Coping the formated file  " + tempName + " into the name of " + designXML)
	shutil.copyfile(designPath + tempName, designPath + designXML)
	return 0
	
def upgradeDesign(additionalParam):
	"""Method for adjusting Design.xml for a new Design.xsd when updating to a new version of the Framework"""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py upgrade_design")
	print("Formatting your design file ...")
	returnCode = formatDesign()
	if returnCode != 0:
		print("There was a problem generating " + output + "; Return code = " + str(returnCode))
		return returnCode
	
	output = "Design.xml.upgraded"
	returnCode = transformDesignVerbose(designPath + "designToUpgradedDesign.xslt", designPath + output, 0, 0, additionalParam)
	
	print("Formatting the upgraded file ")
	formatedOutput = output + ".formatted"
	if platform.system() == "Windows":
		returnCode = subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), designPath + output], designPath + formatedOutput, getCommand("xmllint"))
	elif platform.system() == "Linux":
		returnCode = subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--format", designPath + output], designPath + formatedOutput, getCommand("xmllint"))
	if returnCode != 0:
		print("There was a problem formatting the upgraded file; Return code = " + str(returnCode))
		return returnCode
		
	print("Now running merge-tool. Please merge the upgraded changed")
	returnCode = subprocessWithImprovedErrors([getCommand("diff"), "-o", designPath + designXML, designPath + designXML, designPath + formatedOutput], getCommand("diff"))
	if returnCode != 0:
		print("There was a problem with kdiff3; Return code = " + str(returnCode))
		return returnCode
	return 0
	
def createDiagram(detailLevel=0):
	"""Creates an UML diagram based on the classes of the server.
	
	Keyword arguments:
	detailLevel -- Detail level of the diagram. If it is not present, 0 will be assumed
	"""
	if "quasarGUI.py" in __main__.__file__:
		print("Calling: python quasar.py generate diagram")
	if detailLevel == "":
		detailLevel = 0
	output = "Design.dot"
	returnCode = transformDesignVerbose(designPath + "designToDot.xslt", designPath + output, 0, 1, "detailLevel=" + str(detailLevel))
	print("Generating pdf diagram with dot.")
	returnCode = subprocessWithImprovedErrors([getCommand("graphviz"), "-Tpdf", "-o", designPath + "diagram.pdf", designPath + "Design.dot"], "GraphViz (dot)")
	if returnCode != 0:
		print("There was a problem generating pdf diagram with dot; Return code = " + str(returnCode))
		return returnCode
	return 0
			
