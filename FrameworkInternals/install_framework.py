#!/usr/bin/env python3
# encoding: utf-8
'''
install_framework.py

@author:     Paris Moschovakos <paris.moschovakos@cern.ch>
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
from manage_files import mfInstall, find_missing_source_files
from DesignInspector import DesignInspector
from quasar_basic_utils import yes_or_no, get_quasar_version, print_logo
from deviceClassAnalyzer import addOverrideToDeviceClasses

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

def __print_post_install_notes():
    print(f'Welcome to quasar {Style.BRIGHT}{get_quasar_version()}{Style.RESET_ALL} !')
    print()
    print('For reference on using quasar see:')
    print('1. Webpage: http://quasar.docs.cern.ch,')
    print('2. YouTube tutorials: https://www.youtube.com/watch?v=v212aPmbahM&list=PLz6bxFrT1-KBZxoSxr4ZvlTyxNeYE3L7b')
    print('3. Wiki on our GitHub page: https://github.com/quasar-team/quasar/wiki')
    print('')
    print('For support, contact quasar-developers@cern.ch')
    print()
    print(f'Good luck with creating your OPCUA software with quasar {Style.BRIGHT + Fore.GREEN};-){Style.RESET_ALL}')
    print()

def upgradeProject(destination):
    """Upgrades the framework in a given directory

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """
    if os.path.exists(destination) and os.path.exists(destination + os.path.sep + 'Device') and os.path.exists(destination + os.path.sep + 'Design') and os.path.exists(destination + os.path.sep + 'Configuration') and os.path.exists(destination + os.path.sep + 'AddressSpace'):
        destination = os.path.abspath(destination)
        print( "Selected installation folder: " + destination)
    else:
        print('The chosen directory doesn\'t seem to contain a quasar project or the project is somehow broken.')

    incumbent_quasar_version = get_quasar_version(destination)
    target_quasar_version = get_quasar_version()
    print((f'You are upgrading from version {Style.BRIGHT}{Fore.GREEN}{incumbent_quasar_version}{Style.RESET_ALL}'
           f' to {Style.BRIGHT}{Fore.GREEN}{target_quasar_version}{Style.RESET_ALL}. \n'
           f"It is advised that you read quasar's http://quasar.docs.cern.ch/ChangeLog.html and check what "
           f'needs to be done for upgrade between current and target version.'))
    yn = yes_or_no('Have you read the changelog and understood the remarks and want to upgrade?')
    if yn == 'n':
        print('User cancelled the upgrade, no changes were made, quitting.')
        return

    # Pre-flight: verify the source quasar tree (cwd) has every framework file
    # declared install:overwrite / install:copy_if_not_existing in files.txt.
    # The common failure mode this guards against is an uninitialized LogIt/
    # submodule, which would otherwise cause Installer.install() to abort
    # part-way through and leave the destination project in a half-upgraded state.
    source_directory = os.getcwd()
    missing = find_missing_source_files(source_directory)
    if missing:
        print(f'{Fore.RED}Source quasar tree at {source_directory} is incomplete.{Style.RESET_ALL}')
        print(f'{Fore.RED}{len(missing)} file(s) declared in FrameworkInternals/files.txt are missing on disk:{Style.RESET_ALL}')
        for m in missing[:10]:
            print(f'  - {m}')
        if len(missing) > 10:
            print(f'  ... and {len(missing) - 10} more')
        if any(m.startswith('LogIt' + os.sep) or m.startswith('LogIt/') for m in missing):
            print(f'{Fore.YELLOW}Hint: LogIt is a git submodule. From the quasar source tree, run:{Style.RESET_ALL}')
            print(f'  git -C {source_directory} submodule update --init --recursive')
        print(f'{Fore.RED}Aborting. No changes were made to {destination}.{Style.RESET_ALL}')
        return

    installFramework(destination)
    fix_empty_project_short_name(destination)
    # Add 'override' keyword to D<Class> methods that override Base_D virtuals.
    # Base_D now provides virtual defaults (since p9999), so 'override' ensures
    # compile-time safety when design elements are removed or changed.
    print(f'\n{Style.BRIGHT}Upgrading device classes (adding override keywords)...{Style.RESET_ALL}')
    addOverrideToDeviceClasses({'projectSourceDir': destination})
    print_logo(skip_top=2, skip_bottom=2)
    print(f'Your quasar project in {Style.BRIGHT}{destination}{Style.RESET_ALL} was upgraded')
    print()
    __print_post_install_notes()

def createProject(destination):
    """Installs the framework in a given directory. If the directory doesn't exist, it gets created

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """

    if os.path.exists(destination):
        print(f'The chosen directory {Fore.RED}already exists{Style.RESET_ALL} so we can\'t create a new project there.')
        print(f'If the existing directory already contains a quasar project, try {Style.BRIGHT}upgrade_project{Style.RESET_ALL} command instead.')
        return
    destination = os.path.abspath(destination)
    os.makedirs(destination)
    print('Created installation folder: ' + destination)
    installFramework(destination)
    print_logo(skip_top=2, skip_bottom=2)
    print(f'A new quasar project was created in {Style.BRIGHT}{destination}{Style.RESET_ALL}')
    print()
    __print_post_install_notes()

def installFramework(destination):
    """Installs or upgrades the framework in a given directory

    Keyword arguments:
    destination -- The target directory where the framework will be installed or upgraded
    """

    baseDirectory = os.getcwd()

    print("Calling mfInstall")
    mfInstall(baseDirectory, destination)
