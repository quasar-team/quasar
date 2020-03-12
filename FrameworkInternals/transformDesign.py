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

@contact:    quasar-developers@cern.ch
'''

import os
import os.path
import sys
import filecmp
from externalToolCheck import subprocessWithImprovedErrors
from commandMap import getCommand
import astyleSubstitute
import subprocess
import enum
import jinja2
from colorama import Fore, Style
import pdb
from DesignInspector import DesignInspector
from Oracle import Oracle

# here we define all transforms of Design known to Quasar
# first, IDs
@enum.unique
class TransformKeys(enum.Enum):
    AS_SOURCEVARIABLES_H = 1
    AS_SOURCEVARIABLES_CPP = 2
    AS_CLASS_H = 3
    AS_CLASS_CPP_ALL = 4
    AS_INFOMODEL_H = 5
    AS_INFOMODEL_CPP = 6
    CONFIGURATION_XSD = 7
    CONFIGURATOR = 8
    CONFIG_VALIDATOR = 9
    DESIGN_VALIDATION = 10
    UPGRADE_DESIGN = 11
    CREATE_DIAGRAM_DOT = 12
    D_ROOT_H = 13
    D_ROOT_CPP = 14
    D_BASE_H = 15
    D_BASE_CPP_ALL = 16
    D_DEVICE_H = 17
    D_DEVICE_CPP = 18
    HONKYTONK = 19
    AS_CMAKE = 20
    D_CMAKE = 21
    CONFIG_DOCUMENTATION = 22
    AS_DOCUMENTATION = 23

# column IDs
@enum.unique
class FieldIds(enum.Enum):
    KEY = 0
    TRANSFORM_PATH = 1  # the path to either XSLT or Jinja2 transform
    OUT_PATH = 2
    SOURCE_OR_BINARY = 3
    CPP_FORMAT = 4
    REQUIRES_MERGE = 5
    ADDITIONAL_PARAM = 6


QuasarTransforms = [
    #(0)key                                 (1)where XSLT is                                        (2)output                                       (3) source or b (4)c++format    (5)req merge  (6)additional params
    [TransformKeys.AS_SOURCEVARIABLES_H,    'AddressSpace/designToSourceVariablesHeader.xslt',       'AddressSpace/include/SourceVariables.h',       'B',            True,           False,        None],
    [TransformKeys.AS_SOURCEVARIABLES_CPP,  'AddressSpace/designToSourceVariablesBody.xslt',         'AddressSpace/src/SourceVariables.cpp',         'B',            True,           False,        None],
    [TransformKeys.AS_CLASS_H,              ['AddressSpace','designToClassHeader.jinja'],             'AddressSpace/include/AS{className}.h',         'B',            True,           False,        ['className']],
    [TransformKeys.AS_CLASS_CPP_ALL,        'AddressSpace/designToClassBody.xslt',                   'AddressSpace/src/AddressSpaceClasses.cpp',     'B',            True,           False,        None],
    [TransformKeys.AS_INFOMODEL_H,          'AddressSpace/designToInformationModelHeader.xslt',      'AddressSpace/include/ASInformationModel.h',    'B',            True,           False,        None],
    [TransformKeys.AS_INFOMODEL_CPP,        'AddressSpace/designToInformationModelBody.xslt',        'AddressSpace/src/ASInformationModel.cpp',      'B',            True,           False,        None],
    [TransformKeys.AS_CMAKE,                ['AddressSpace','designToGeneratedCmakeAddressSpace.jinja'], 'AddressSpace/cmake_generated.cmake',           'B',            False,          False,        None],
    [TransformKeys.CONFIGURATION_XSD,       'Configuration/designToConfigurationXSD.xslt',           'Configuration/Configuration-noxinclude.xsd',   'B',            False,          False,        'metaXsdPath={metaXsdPath}'],
    [TransformKeys.CONFIGURATOR,            'Configuration/designToConfigurator.xslt',               'Configuration/Configurator.cpp',               'B',            True,           False,        None],
    [TransformKeys.CONFIG_VALIDATOR,        'Configuration/designToConfigValidator.xslt',            'Configuration/ConfigValidator.cpp',            'B',            True,           False,        None],
    [TransformKeys.DESIGN_VALIDATION,       'Design/designValidation.xslt',                          'Design/validationOutput.removeme',             'B',            False,          False,        None],
    [TransformKeys.UPGRADE_DESIGN,          'Design/designToUpgradedDesign.xslt',                    'Design/Design.xml.upgraded',                   'S',            False,          False,        '{whatToDo}'],
    [TransformKeys.CREATE_DIAGRAM_DOT,      'Design/designToDot.xslt',                               'Design/Design.dot',                            'B',            False,          False,        'detailLevel={detailLevel}'],
    [TransformKeys.D_ROOT_H,                ['Device','designToRootHeader.jinja'],                   'Device/include/DRoot.h',                       'B',            True,           False,        None],
    [TransformKeys.D_ROOT_CPP,              ['Device','designToRootBody.jinja'],                     'Device/src/DRoot.cpp',                         'B',            True,           False,        None],
    [TransformKeys.D_BASE_H,                ['Device','designToDeviceBaseHeader.jinja'],             'Device/generated/Base_D{className}.h',         'B',            True,           False,        ['className']],
    [TransformKeys.D_BASE_CPP_ALL,          ['Device','designToDeviceBaseBody.jinja'],               'Device/generated/Base_All.cpp',                'B',            True,           False,        None],
    [TransformKeys.D_DEVICE_H,              ['Device','designToDeviceHeader.jinja'],                 'Device/include/D{className}.h',                'S',            True,           True,         ['className']],
    [TransformKeys.D_DEVICE_CPP,            'Device/designToDeviceBody.xslt',                        'Device/src/D{className}.cpp',                  'S',            True,           True,         'className={className}'],
    [TransformKeys.D_CMAKE,                 ['Device','designToGeneratedCmakeDevice.jinja'],         'Device/generated/cmake_header.cmake',          'B',            False,          False,        None],
    [TransformKeys.HONKYTONK,               'Extra/designToHonkyTonk.xslt',                          'Extra/honkyTonky.cc',                          'S',            True,           False,        None],
    [TransformKeys.CONFIG_DOCUMENTATION,    'Configuration/designToConfigDocumentationHtml.xslt',    'Documentation/ConfigDocumentation.html',       'S',            False,          False,        None],
    [TransformKeys.AS_DOCUMENTATION,        'AddressSpace/designToAddressSpaceDocHtml.xslt',         'Documentation/AddressSpaceDoc.html',           'S',            False,          False,        None]
    ]

def transformDesignVerbose(transformPath, outputFile, requiresMerge, astyleRun=False, additionalParam=None):
    """Just a verbose wrapper around transformDesign, for arguments description see transformDesign below.
       Note this is agnostic to transformation type (XSLT or Jinja2) """
    print("Using the transform [" + transformPath + "] to generate the file [" + outputFile + "] {0}"
                .format('additionalParam=[{0}]'.format(additionalParam) if additionalParam is not None else ''))
    return transformDesign(transformPath, outputFile, requiresMerge, astyleRun, additionalParam)

def transformDesignByXslt(designXmlPath, transformPath, outputFile, additionalParam):
    XSLT_JAR = '.' + os.path.sep + 'Design' + os.path.sep + getCommand('saxon')
    subprocessWithImprovedErrors([getCommand('java'), '-jar', XSLT_JAR, designXmlPath, transformPath, '-o:' + outputFile] + additionalParam, getCommand("java"))

def capFirst(s):
    return s[0].upper() + s[1:]

def templateDebug(text):
    print(Fore.MAGENTA + str(text) + Style.RESET_ALL)
    return ''

def transformDesignByJinja(designXmlPath, transformPath, outputFile, additionalParam):
    """ additionalParam - a dictionary that will be passed to the transform """
    outputDirectory = os.path.dirname(outputFile)
    if not os.path.isdir(outputDirectory):
        os.makedirs(outputDirectory)
    designInspector = DesignInspector(designXmlPath)
    commonTemplatesLoader = jinja2.FileSystemLoader('/home/pnikiel/gitProjects/OPCUA-1746_gen_using_jinja2/Common/templates')  # TODO!
    moduleTemplatesLoader = jinja2.FileSystemLoader(os.path.dirname(transformPath))
    env = jinja2.Environment(loader=jinja2.ChoiceLoader([commonTemplatesLoader, moduleTemplatesLoader]))
    env.filters['capFirst']  = capFirst
    env.filters['debug'] = templateDebug
    env.trim_blocks = True
    fout = open(outputFile, 'w')
    render_args = {'designInspector':designInspector, 'oracle':Oracle()}
    if not isinstance(additionalParam, dict):
        render_args.update({'additionalParam':additionalParam})
    else:
        render_args.update(additionalParam)
    fout.write(env.get_template(os.path.basename(transformPath)).render(render_args).encode('utf-8'))
    print(Fore.BLUE + 
        'quasar Jinja2 generator: Generated {0}, wrote {1} bytes'.format(outputFile, fout.tell()) +
        Style.RESET_ALL)
    

def transformDesign(transformPath, outputFile, requiresMerge, astyleRun, additionalParam=None):
    """Generates a file, applying a transform (XSLT or Jinja2) to Design.xml

    Keyword arguments:
    transformPath        -- transform file where the transformation is defined, either XSLT or Jinja2
    outputFile           -- name of the file to be generated
    requiresMerge        -- if True, will prevent from overwriting output filename, running merge-tool
    astyleRun            -- if True, will run astyle on generated file
    additionalParam      -- Optional extra param to be passed e.g. to XSLT transform.
    """
    if isinstance(additionalParam, str):
        additionalParam = [additionalParam] # compat mode for passing multiple additional params
    elif additionalParam == None:
        additionalParam = []

    # files
    designXmlPath = '.' + os.path.sep + 'Design' + os.path.sep + 'Design.xml'  # TODO os.path.join
    if requiresMerge:
        originalOutputFile = outputFile
        outputFile = outputFile + '.generated'
    try:
        if transformPath.endswith('.jinja'):
            transformDesignByJinja(designXmlPath, transformPath, outputFile, additionalParam)
        elif transformPath.endswith('.xslt'):
            transformDesignByXslt(designXmlPath, transformPath, outputFile, additionalParam)
        else:
            raise Exception("Couldnt determine transformation type")

        if astyleRun:
            try:
                subprocess.call([getCommand('astyle'), outputFile])
            except Exception as e:
                try:
                    subprocess.call([getCommand('indent'), outputFile, '-o', outputFile])
                    print("We indented your code using 'indent' tool which is a fall-back tool. For best user satisfaction, please install astyle as described in the quasar documentation.  ")
                except Exception as e:
                    print("We didnt manage to run neither 'astyle' nor 'indent' tool. We're running a fall-back tool now. For best user satisfaction, please install astyle as described in the quasar documentation.  ")
                    astyleSubstitute.do_indentation(outputFile)



        if requiresMerge:
            # If the file existed previously and it is different from the old one we run kdiff3
            if (os.path.isfile(originalOutputFile)) and (filecmp.cmp(originalOutputFile, outputFile) == False):
                subprocessWithImprovedErrors([getCommand('diff'), "-o", originalOutputFile, originalOutputFile, outputFile], getCommand("diff"), [0, 1])  # 1 is a valid return, since it means that the user quitted without saving the merge, and this is still ok.
            else:  # If the file didn't exist before, or it is equal to the old one, we rename the generated file to have the proper name
                if os.path.isfile(originalOutputFile):
                    os.remove(originalOutputFile)
                os.rename(outputFile, originalOutputFile)
    except Exception:
        if os.path.isfile(outputFile):
            print("Removing partially generated file: {0}".format(outputFile))
            os.remove(outputFile)  # sometimes the output of XSLT processor is partial...
        raise

def getTransformSpecByKey(key):
    return [x for x in QuasarTransforms if x[FieldIds.KEY.value]==key][0]

def getTransformOutput (key, supplementaryData={}):
    """Returns absolute path to the output of transform identified by key 'key'"""
    transformSpec = getTransformSpecByKey(key)
    outputDir = supplementaryData['context']['projectBinaryDir'] if transformSpec[FieldIds.SOURCE_OR_BINARY.value] == 'B' else supplementaryData['context']['projectSourceDir']
    outputFileRaw = transformSpec[FieldIds.OUT_PATH.value].format(**supplementaryData)
    return os.path.join(outputDir, outputFileRaw)

def transformByKey (keys, supplementaryData={}):
    """ This runs the transform both for a single key as well as a list of keys. 
        keys              - a key from TransformKeys enum, or a list of such keys
        supplementaryData - a dictionary with (at minimum) 'context' key, 
                            remaining keys are typically className or whatever 
                            comes from command arguments. """
    if isinstance(keys, list): # this is to run multiple transforms
        for key in keys:
            transformByKey(key, supplementaryData) # recurrent call
    else:
        transformSpec = getTransformSpecByKey(keys)
        outputDir = supplementaryData['context']['projectBinaryDir'] if transformSpec[FieldIds.SOURCE_OR_BINARY.value] == 'B' else supplementaryData['context']['projectSourceDir']
        outputFile = getTransformOutput(keys, supplementaryData)
        if isinstance(transformSpec[FieldIds.ADDITIONAL_PARAM.value], str): 
            # this case is for XSLT where args are passed as string A=X B=Y etc
            additionalParam = transformSpec[FieldIds.ADDITIONAL_PARAM.value].format(**supplementaryData)
        elif isinstance(transformSpec[FieldIds.ADDITIONAL_PARAM.value], list):
            # this case is to pass a sub-dict of supplementaryData wich chosen keys, applies to Jinja2
            additionalParam = { x: supplementaryData[x] for x in transformSpec[FieldIds.ADDITIONAL_PARAM.value]}
        else:
            additionalParam = None
        if isinstance(transformSpec[FieldIds.TRANSFORM_PATH.value], list):
            tp = transformSpec[FieldIds.TRANSFORM_PATH.value]
            transformPath = os.path.join(tp[0], 'templates', tp[1])
        else:
            transformPath = transformSpec[FieldIds.TRANSFORM_PATH.value]
        transformDesignVerbose(
            transformPath = transformPath,
            outputFile = outputFile,
            requiresMerge = transformSpec[FieldIds.REQUIRES_MERGE.value],
            astyleRun = transformSpec[FieldIds.CPP_FORMAT.value],
            additionalParam = additionalParam)
