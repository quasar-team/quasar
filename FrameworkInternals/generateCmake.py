#!/usr/bin/env python3
# encoding: utf-8
'''
generateCmake.py

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
import platform
from transformDesign import TransformKeys, transformByKey
from externalToolCheck import subprocessWithImprovedErrors
from commandMap import getCommand
from quasarExceptions import Mistake
from manage_files import symlinkRuntimeDeps
from quasar_basic_utils import extract_argument
from merge_design_and_meta import merge_user_and_meta_design

BuilderDefault = "PlatformDefault"

# previous definition: def generateCmake(context, buildType="Release", buildSystem=BuilderDefault):
def generateCmake(context, *args):
    """Generates CMake header lists in various directories, and then calls cmake.

    Keyword arguments:
    buildType -- Optional parameter to specify Debug or Release build. If it is not specified it will default to Release.
    """
    args = list(args) # we do it to be able to remove things via extract_argument, otherwise it is immutable tuple
    (args2, builder) = extract_argument(args, "--builder")
    if len(args2) > 1:
        raise WrongArguments("Max one positional argument for generateCmake")
    buildType = "Release" if len(args2) == 0 else args2[0]
    if builder is None: builder = BuilderDefault

    if not context['projectSourceDir'] == context['projectBinaryDir']: # out-of-source build
        if os.path.isfile(os.path.join(context['projectSourceDir'], 'CMakeCache.txt')):
            raise Mistake('User mistake? CMakeCache.txt exists in source directory; '+
                                      'that will prevent CMake to make a successful out-of-source build. '+
                                      'Remove CMakeCache.txt and all cmake_install.cmake files, and build/ directory')

    projectSourceDir = context['projectSourceDir']
    projectBinaryDir = context['projectBinaryDir']
    if not os.path.isdir(projectBinaryDir):
        print("PROJECT_BINARY_DIR {0} doesn't exist -- creating it.".format(projectBinaryDir))
        os.mkdir(projectBinaryDir)
        os.mkdir(os.path.join(projectBinaryDir, "bin"))
        print("Creating symlinks for xml files in the build directory")
        symlinkRuntimeDeps(context, '*.xml')
        symlinkRuntimeDeps(context, 'ServerConfig.xsd')

    projectSourceDesignDir = os.path.join(projectBinaryDir, 'Design')
    if not os.path.isdir(projectSourceDesignDir):
        os.mkdir(projectSourceDesignDir)
    merge_user_and_meta_design(
        user_file   = open(mode='r', encoding='utf-8', file=os.path.join(projectSourceDir, 'Design', 'Design.xml')),
        meta_file   = open(mode='r', encoding='utf-8', file=os.path.join(projectSourceDir, 'Meta',   'design', 'meta-design.xml')),
        merged_file = open(mode='w', encoding='utf-8', file=os.path.join(projectBinaryDir, 'Design', 'DesignWithMeta.xml'))
    )

    transformByKey(TransformKeys.AS_CMAKE,     {'context':context})
    transformByKey(TransformKeys.D_BASE_CMAKE, {'context':context})
    transformByKey(TransformKeys.D_CMAKE,      {'context':context})
    print("Build type [{0}], Builder [{1}]".format(buildType, builder))
    os.chdir(projectBinaryDir)

    print("Calling CMake")
    if platform.system() == "Windows":
        subprocessWithImprovedErrors([getCommand("cmake"), "-DCMAKE_BUILD_TYPE=" + buildType,
                                      "-G", "Visual Studio 15 2017 Win64", projectSourceDir],
                                     getCommand("cmake"))
    elif platform.system() == "Linux":
        builderArgs = [] if builder == BuilderDefault else ["-G", builder]
        subprocessWithImprovedErrors([getCommand("cmake"), "-DCMAKE_BUILD_TYPE=" + buildType] + builderArgs +
                                      [projectSourceDir],
                                     getCommand("cmake"))
