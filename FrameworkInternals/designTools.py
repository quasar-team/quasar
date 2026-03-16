#!/usr/bin/env python3
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
from transformDesign import transformByKey, TransformKeys, getTransformOutput
from externalToolCheck import subprocessWithImprovedErrors
from externalToolCheck import subprocessWithImprovedErrorsPipeOutputToFile
from commandMap import getCommand
from DesignValidator import DesignValidator

designPath = "Design" + os.path.sep
designXML = "Design.xml"
designXSD = "Design.xsd"

def validateDesign(context):
    """Checks quasar's design validity"""
    design_validator = DesignValidator(
        os.path.sep.join(['Design', designXSD]), 
        os.path.sep.join(['Design', designXML]))
    design_validator.validate()

def formatXml(inFileName, outFileName):
    if platform.system() == "Windows":
        subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), inFileName], outFileName, getCommand("xmllint"))
    elif platform.system() == "Linux":
        subprocessWithImprovedErrorsPipeOutputToFile([getCommand("xmllint"), "--format", inFileName], outFileName, getCommand("xmllint"))

def formatDesign():
    """Formats design.xml. This is done to have always the same indentation format. The formatting is done in a separate file, in case something goes wrong, and then copied over."""
    backupName = designXML + ".backup"
    tempName = designXML + ".new"

    print("Creating a backup of " + designXML + " under the name of " + backupName)
    shutil.copyfile(designPath + designXML, designPath + backupName)

    print("Formatting the file " + designXML + "using the tool XMLlint. The result will be saved in " + tempName)
    formatXml(designPath + designXML, designPath + tempName)

    print("Copying the formated file  " + tempName + " into the name of " + designXML)
    shutil.copyfile(designPath + tempName, designPath + designXML)

def upgradeDesign(context, additionalParam):
    """Method for adjusting Design.xml for a new Design.xsd when updating to a new version of the Framework"""
    print("Formatting your design file ...")
    formatDesign()

    transformByKey(TransformKeys.UPGRADE_DESIGN, {'context':context, 'whatToDo':additionalParam})

    print("Formatting the upgraded file ")
    upgradedNonFormatted = getTransformOutput(TransformKeys.UPGRADE_DESIGN, {'context':context} )
    upgradedFormatted = upgradedNonFormatted + ".formatted"

    formatXml(upgradedNonFormatted, upgradedFormatted)

    print("Now running merge-tool. Please merge the upgraded changed")
    subprocessWithImprovedErrors([getCommand("diff"), "-o", designPath + designXML, designPath + designXML, upgradedFormatted], getCommand("diff"))

def createDiagram(context, detailLevel=0, mode='dot'):
    """Creates an UML diagram based on the classes of the server.

    Keyword arguments:
    detailLevel -- Detail level of the diagram. If it is not present, 0 will be assumed
    mode -- one of graph layout modes to be passed to graphviz
    """
    graphvizArgs = {
            'dot':[],
            'circo':['-Kcirco', '-Gsplines=true', '-Granksep=0.1', '-Gmindist=0', '-Goverlap=false', '-Gepsilon=0.00001'],
            'fdp':['-Kfdp', '-Gsplines=true', '-Goverlap=false', '-Gepsilon=0.00001', '-GK=0.01', '-Gmaxiter=10000', '-Gstart=random']
    }
    print('Using {0} as level of detail'.format(str(detailLevel)))
    print('Using {0} as layout mode. Hint: from quasar 1.3.5, you can choose among: {1}, run with -h for help.'.format(mode, ','.join(graphvizArgs.keys())))
    transformByKey(TransformKeys.CREATE_DIAGRAM_DOT, {'context': context, 'detailLevel':detailLevel})
    args = ["-Tpdf", "-o", designPath + "diagram.pdf", getTransformOutput(TransformKeys.CREATE_DIAGRAM_DOT, {'context': context})] + graphvizArgs[mode]
    subprocessWithImprovedErrors([getCommand("graphviz")] + args , "GraphViz (dot)")

