#!/usr/bin/env python3
# encoding: utf-8
'''
install_framework.py

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
import re
import shutil
from colorama import Fore, Style
from manage_files import mfInstall
from DesignInspector import DesignInspector
from quasar_basic_utils import yes_or_no, get_quasar_version

def fix_empty_project_short_name(destination):
    '''Fixes empty attribute of projectShortName in Design, now required since quasar nebula.B1'''
    design_path = os.path.join(destination, 'Design', 'Design.xml')
    di = DesignInspector(design_path)
    project_name = di.getProjectName()
    if project_name == "":
        print(('It seems that you are upgrading a project with projectShortName set to empty '
               'string. That was well permitted in the past but now we require some decent name. '
               'Should we try and fix it now? Type y or n. If you dont fix it (either now or later '
               'manually editing Design.xml) the project wont build.\n'))
        yn = yes_or_no('Shall we try to fix it?')
        if yn == 'y':
            print(('Ok, cool. So what short name should we pick? Note: whitespaces are not allowed.'
                   ' A good candidate would be sth like OpcUaEngineControllerServer, '
                   ' my_first_quasar_project or something alike to well identify your project'))
            new_name = input()
            re_valid_name = re.compile(r'[a-zA-Z0-9_\-]+')
            if re_valid_name.match(new_name) is None:
                print('Chosen name did not fit the valid pattern. Run the upgrade again')
                return
            design_element = di.xpath('/d:design')[0]
            design_element.attrib['projectShortName'] = new_name
            shutil.copy(design_path, design_path+'.backup')
            di.tree.write(os.path.join(destination, 'Design', 'Design.xml'),
                             xml_declaration=True,
                             encoding='UTF-8')
            print(f'projectShortName was updated to: {new_name}')
            print((f'Your Design.xml was modified. Backup was stored with suffix .backup. Please '
                   f'review and commit the change.'))
        else:
            print('Remember: you have to do it yourself ;-) ')

def upgradeProject(destination):
    """Upgrades the framework in a given directory

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """
    if os.path.exists(destination) and os.path.exists(destination + os.path.sep + 'Device') and os.path.exists(destination + os.path.sep + 'Design') and os.path.exists(destination + os.path.sep + 'Configuration') and os.path.exists(destination + os.path.sep + 'AddressSpace'):
        destination = os.path.abspath(destination)
        print( "Selected installation folder: " + destination)
    else:
        raise Exception("There was a problem when trying to ugrade a project, invalid path.")

    incumbent_quasar_version = get_quasar_version(destination)
    target_quasar_version = get_quasar_version()
    print((f'You are upgrading from version {Fore.BLUE}{incumbent_quasar_version}{Style.RESET_ALL}'
           f' to {Fore.BLUE}{target_quasar_version}{Style.RESET_ALL}. \n'
           f"It is advised that you read quasar's Documentation/ChangeLog.html and check what "
           f'needs to be done for upgrade between current and target version.'))
    yn = yes_or_no('Have you read the changelog and understood the remarks and want to upgrade?')
    if yn == 'n':
        print('User cancelled the upgrade, no changes were made, quitting.')
        return
    installFramework(destination)
    fix_empty_project_short_name(destination)

def createProject(destination):
    """Installs the framework in a given directory. If the directory doesn't exist, it gets created

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """
    if os.path.exists(destination):
        destination = os.path.abspath(destination)
        print( "Selected installation folder: " + destination)
    else:
        destination = os.path.abspath(destination)
        os.makedirs(destination)
        print( "Created installation folder: " + destination)
    installFramework(destination)

def installFramework(destination):
    """Installs or upgrades the framework in a given directory

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """

    baseDirectory = os.getcwd()

    print("Calling mfInstall")
    mfInstall(baseDirectory, destination)
