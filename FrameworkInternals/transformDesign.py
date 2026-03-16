#!/usr/bin/env python3
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
import errno
import sys
import filecmp
from externalToolCheck import subprocessWithImprovedErrors
from commandMap import getCommand
import subprocess
import enum
import jinja2
from colorama import Fore, Style
from DesignInspector import DesignInspector
from Oracle import Oracle
import transform_filters

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
    CREATE_DIAGRAM_DOT = 12
    D_ROOT_H = 13
    D_ROOT_CPP = 14
    D_BASE_H = 15
    D_BASE_CPP_ALL = 16
    D_DEVICE_H = 17
    D_DEVICE_CPP = 18
    AS_CMAKE = 20
    D_BASE_CMAKE = 21
    D_CMAKE = 22 
    CONFIG_DOCUMENTATION = 23
    AS_DOCUMENTATION = 24
    CONFIG_DOCUMENTATION_MD = 25
    AS_DOCUMENTATION_MD = 26

# column IDs
@enum.unique
class FieldIds(enum.Enum):
    KEY = 0
    TRANSFORM_PATH = 1  # the path to either XSLT or Jinja2 transform
    DESIGN_WITH_META = 2 # if True transform uses design merged with meta, otherwise only user design
    OUT_PATH = 3
    SOURCE_OR_BINARY = 4
    CPP_FORMAT = 5
    REQUIRES_MERGE = 6
    ADDITIONAL_PARAM = 7


QuasarTransforms = [
    #(0)key                                 (1)where is the transform                                     (2)meta   (3)output                                          (4) source or b (5)c++format    (6)req merge  (7)additional params
    [TransformKeys.AS_SOURCEVARIABLES_H,    ['AddressSpace','designToSourceVariablesHeader.jinja'],       True,     'AddressSpace/include/SourceVariables.h',          'B',            True,           False,        None],
    [TransformKeys.AS_SOURCEVARIABLES_CPP,  ['AddressSpace','designToSourceVariablesBody.jinja'],         True,     'AddressSpace/src/SourceVariables.cpp',            'B',            True,           False,        None],
    [TransformKeys.AS_CLASS_H,              ['AddressSpace','designToClassHeader.jinja'],                 True,     'AddressSpace/include/AS{className}.h',            'B',            True,           False,        ['className']],
    [TransformKeys.AS_CLASS_CPP_ALL,        ['AddressSpace','designToClassBody.jinja'],                   True,     'AddressSpace/src/AddressSpaceClasses.cpp',        'B',            True,           False,        None],
    [TransformKeys.AS_INFOMODEL_H,          ['AddressSpace','designToInformationModelHeader.jinja'],      True,     'AddressSpace/include/ASInformationModel.h',       'B',            True,           False,        None],
    [TransformKeys.AS_INFOMODEL_CPP,        ['AddressSpace','designToInformationModelBody.jinja'],        True,     'AddressSpace/src/ASInformationModel.cpp',         'B',            True,           False,        None],
    [TransformKeys.AS_CMAKE,                ['AddressSpace','designToGeneratedCmakeAddressSpace.jinja'],  True,     'AddressSpace/cmake_generated.cmake',              'B',            False,          False,        None],
    [TransformKeys.CONFIGURATION_XSD,       ['Configuration','designToConfigurationXSD.jinja'],           True,     'Configuration/Configuration-noxinclude.xsd',      'B',            False,          False,        None],
    [TransformKeys.CONFIGURATOR,            ['Configuration','designToConfigurator.jinja'],               True,     'Configuration/Configurator.cpp',                  'B',            True,           False,        None],
    [TransformKeys.CONFIG_VALIDATOR,        ['Configuration','designToConfigValidator.jinja'],            True,     'Configuration/ConfigValidator.cpp',               'B',            True,           False,        None],
    [TransformKeys.CREATE_DIAGRAM_DOT,      ['Design','designToDot.jinja'],                               False,    'Design/Design.dot',                               'B',            False,          False,        ['detailLevel']],
    [TransformKeys.D_ROOT_H,                ['Device','designToRootHeader.jinja'],                        True,     'Device/include/DRoot.h',                          'B',            True,           False,        None],
    [TransformKeys.D_ROOT_CPP,              ['Device','designToRootBody.jinja'],                          True,     'Device/src/DRoot.cpp',                            'B',            True,           False,        None],
    [TransformKeys.D_BASE_H,                ['Device','designToDeviceBaseHeader.jinja'],                  True,     'Device/generated/Base_D{className}.h',            'B',            True,           False,        ['className']],
    [TransformKeys.D_BASE_CPP_ALL,          ['Device','designToDeviceBaseBody.jinja'],                    True,     'Device/generated/Base_All.cpp',                   'B',            True,           False,        None],
    [TransformKeys.D_DEVICE_H,              ['Device','designToDeviceHeader.jinja'],                      False,    'Device/include/D{className}.h',                   'S',            True,           True,         ['className']],
    [TransformKeys.D_DEVICE_CPP,            ['Device','designToDeviceBody.jinja'],                        False,    'Device/src/D{className}.cpp',                     'S',            True,           True,         ['className']],
    [TransformKeys.D_BASE_CMAKE,            ['Device','designToGeneratedCmakeDeviceBase.jinja'],          True,     'Device/generated/cmake_device_base_header.cmake', 'B',            False,          False,        None],
    [TransformKeys.D_CMAKE,                 ['Device','designToGeneratedCmakeDevice.jinja'],              False,    'Device/generated/cmake_device_header.cmake',      'B',            False,          False,        None],
    [TransformKeys.CONFIG_DOCUMENTATION,    ['Configuration','designToConfigDocumentationHtml.jinja'],    False,     'Documentation/ConfigDocumentation.html',          'S',            False,          False,        None],
    [TransformKeys.AS_DOCUMENTATION,        ['AddressSpace','designToAddressSpaceDocHtml.jinja'],         False,     'Documentation/AddressSpaceDoc.html',              'S',            False,          False,        None],
    [TransformKeys.CONFIG_DOCUMENTATION_MD, ['Configuration','designToConfigDocumentationMd.jinja'],      False,     'Documentation/ConfigDocumentation.md',            'S',            False,          False,        None],
    [TransformKeys.AS_DOCUMENTATION_MD,     ['AddressSpace','designToAddressSpaceDocMd.jinja'],           False,     'Documentation/AddressSpaceDoc.md',                'S',            False,          False,        None]
    ]

def transformDesignVerbose(transformPath, designXmlPath, outputFile, requiresMerge, astyleRun=False, additionalParam=None):
    """Just a verbose wrapper around transformDesign, for arguments description see transformDesign below.
       Note this is agnostic to transformation type (XSLT or Jinja2) """
    print("Using the transform [" + transformPath + "] on design file [" + designXmlPath + "] to generate the file [" + outputFile + "] {0}"
                .format('additionalParam=[{0}]'.format(additionalParam) if additionalParam is not None else ''))
    return transformDesign(transformPath, designXmlPath, outputFile, requiresMerge, astyleRun, additionalParam)

def run_indent_tool(unindented_content, fout):
    """Runs indentation tool, preferably astyle.
       unindented_content - stream of bytes (e.g. encoded string) to pass to the tool,
       fout - a file object to which the output will be written"""
    try:
        astyle_version_check_process = subprocess.run(['astyle', '--version'],
                                                          stderr=subprocess.PIPE,
                                                          stdout=subprocess.PIPE,
                                                          check=True)
        # note to line below: whether version is printed in stdout or stderr unfortunately depends
        # on astyle version ... ironic ;-)
        astyle_version_text = astyle_version_check_process.stderr.decode('utf-8') + astyle_version_check_process.stdout.decode('utf-8')
        if 'Artistic Style Version 3' not in astyle_version_text:
            raise Exception(('Found astyle with incompatible version. Astyle versions prior to 3 '
                             'are known to have a bug which makes them impossible to use with '
                             'quasar. The version string returned was: '
                             f'{astyle_version_text}'))
        completed_indenter_process = subprocess.run(['astyle'], input=unindented_content,
                                                    stdout=subprocess.PIPE, check=True)
    except FileNotFoundError:
        print(Fore.YELLOW +
            ("Achtung, achtung. 'astyle' was not found in your system. This violates the "
            "requirements for running quasar. We will try now with 'indent' tool as fallback "
            "(maybe it's Friday afternoon now and you want to go home). "
            "If this message is annoying to you, it's good and intended. Get astyle "
            "installed asap, latest Monday morning.") +
            Style.RESET_ALL)
        try:
            completed_indenter_process = subprocess.run(['indent'], input=unindented_content,
                                                        stdout=subprocess.PIPE, check=True)
        except FileNotFoundError:
            raise Exception(("None of supported indent tools - astyle or indent - was found. Can't "
                             "continue. Please see quasar documentation on organizing "
                             "dependencies."))
    fout.write(completed_indenter_process.stdout)

def handle_abort(msg):
    raise Exception(f'Quasar transform exception:  {msg}') # TODO shall we have a better exc class for it ?

def transformDesignByJinja(designXmlPath, transformPath, outputFile, additionalParam, indent_cpp=False):
    """ additionalParam - a dictionary that will be passed to the transform """
    outputDirectory = os.path.dirname(outputFile)
    try:
        os.makedirs(outputDirectory)
    except OSError as e:
        if e.errno == errno.EEXIST:
            pass # no problem, what matters is it exists.
        else:
            raise
    designInspector = DesignInspector(designXmlPath)
    transformDir = os.path.dirname(transformPath)
    commonTemplatesLoader = jinja2.FileSystemLoader(os.path.join(transformDir, '..', '..', 'Common', 'templates'))
    moduleTemplatesLoader = jinja2.FileSystemLoader(transformDir)
    env = jinja2.Environment(loader=jinja2.ChoiceLoader([commonTemplatesLoader, moduleTemplatesLoader]))
    transform_filters.setup_all_filters(env)
    env.trim_blocks = True
    env.globals['abort'] = handle_abort
    fout = open(outputFile, 'wb')
    render_args = {'designInspector':designInspector, 'oracle':Oracle()}
    if not isinstance(additionalParam, dict):
        render_args.update({'additionalParam':additionalParam})
    else:
        render_args.update(additionalParam)
    unindented_content = env.get_template(os.path.basename(transformPath)).render(render_args).encode('utf-8')
    if indent_cpp:
        run_indent_tool(unindented_content, fout)
        print(Fore.BLUE +
            'quasar Jinja2 generator: Generated+indented {0}, wrote {1} bytes (unindented size: {2})'.format(
                outputFile,
                fout.tell(),
                len(unindented_content)) +
            Style.RESET_ALL)
    else:
        fout.write(unindented_content)
        print(Fore.BLUE +
            'quasar Jinja2 generator: Generated {0}, wrote {1} bytes'.format(outputFile, fout.tell()) +
            Style.RESET_ALL)

def transformDesign(transform_path, designXmlPath, outputFile, requiresMerge, astyleRun, additionalParam=None):
    """Generates a file, applying a transform (XJinja2) to Design.xml

    Keyword arguments:
    transformPath        -- transform file where the transformation is defined, either XSLT or Jinja2
    designXmlPath        -- path to XML file upon which transform will be based
    outputFile           -- name of the file to be generated
    requiresMerge        -- if True, will prevent from overwriting output filename, running merge-tool
    astyleRun            -- if True, will run astyle on generated file
    additionalParam      -- Optional extra param to be passed e.g. to XSLT transform.
    """
    transformPath = transform_path

    processedAdditionalParam = additionalParam if additionalParam is not None else {}

    if requiresMerge:
        originalOutputFile = outputFile
        outputFile = outputFile + '.generated'
    try:
        if transformPath.endswith('.jinja'):
            transformDesignByJinja(designXmlPath, transformPath, outputFile, processedAdditionalParam, astyleRun)
        else:
            raise Exception("Couldnt determine transformation type")
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

def get_transform_path (key):
    transformSpec = getTransformSpecByKey(key)

    if isinstance(transformSpec[FieldIds.TRANSFORM_PATH.value], list):
        # only for NextGen transforms: they must reside in "templates/"
        tp = transformSpec[FieldIds.TRANSFORM_PATH.value]
        transformPath = os.path.join(tp[0], 'templates', tp[1])
    else:
        transformPath = transformSpec[FieldIds.TRANSFORM_PATH.value]
    return transformPath

def get_design_xml(key, supplementaryData={}):
    transformSpec = getTransformSpecByKey(key)
    usesDesignWithMeta = transformSpec[FieldIds.DESIGN_WITH_META.value]

    if usesDesignWithMeta:
        result = os.path.join(supplementaryData['context']['projectBinaryDir'], 'Design', 'DesignWithMeta.xml')
    else:
        result = os.path.join(supplementaryData['context']['projectSourceDir'], 'Design', 'Design.xml')
    print(Fore.BLUE + 
        f"key [{key}] uses design with meta? [{usesDesignWithMeta}] returning design XML path [{result}]" + 
        Style.RESET_ALL)
    return result

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
        if isinstance(transformSpec[FieldIds.ADDITIONAL_PARAM.value], list):
            # this case is to pass a sub-dict of supplementaryData wich chosen keys, applies to Jinja2
            additionalParam = { x: supplementaryData[x] for x in transformSpec[FieldIds.ADDITIONAL_PARAM.value]}
        elif transformSpec[FieldIds.ADDITIONAL_PARAM.value] is None:
            additionalParam = None
        else:
            raise Exception(("The field additional_params in the transform "
                             "table can only be None or a list"))
        transformPath = get_transform_path(keys)
        designXmlPath = get_design_xml(keys, supplementaryData)
        transformDesignVerbose(
            transformPath = transformPath,
            designXmlPath = designXmlPath,
            outputFile = outputFile,
            requiresMerge = transformSpec[FieldIds.REQUIRES_MERGE.value],
            astyleRun = transformSpec[FieldIds.CPP_FORMAT.value],
            additionalParam = additionalParam)
