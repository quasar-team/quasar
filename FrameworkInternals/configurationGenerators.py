#!/usr/bin/env python3
# encoding: utf-8
'''
configurationGenerators.py

@author:     Damian Abalo Miron <damian.abalo@cern.ch>
@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2015 CERN

@license:
Copyright (c) 2015, CERN, Universidad de Oviedo.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted
provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
   and the following disclaimer in the documentation and/or other materials provided with the
   distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS  OR
IMPLIED  WARRANTIES, INCLUDING, BUT NOT  LIMITED TO, THE IMPLIED WARRANTIES  OF  MERCHANTABILITY
AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, EXEMPLARY, OR  CONSEQUENTIAL
DAMAGES (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR  SERVICES; LOSS OF
USE, DATA, OR PROFITS; OR BUSINESS  INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY  OF  LIABILITY,
WHETHER IN  CONTRACT, STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE OR OTHERWISE)  ARISING IN
ANY WAY OUT OF  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''

import os
from transformDesign import TransformKeys, transformByKey, getTransformOutput
from externalToolCheck import subprocessWithImprovedErrorsPipeOutputToFile
from commandMap import getCommand

def generateConfiguration(context):
    """Generates the file Configuration.xsd."""
    config_xsd_path = os.path.join(context['projectBinaryDir'], 'Configuration', 'Configuration.xsd')
    try:
        transformByKey(TransformKeys.CONFIGURATION_XSD, {'context':context})
    except:
        if os.path.isfile(config_xsd_path):
            os.remove(config_xsd_path)
        raise
    subprocessWithImprovedErrorsPipeOutputToFile(
        [getCommand("xmllint"), "--format", "--xinclude",
         getTransformOutput(TransformKeys.CONFIGURATION_XSD, {'context':context})],
        config_xsd_path,
        getCommand("xmllint"))
