#!/usr/bin/env python3
# encoding: utf-8
'''
manage_files.py

@author:     Piotr Nikiel <piotr@nikiel.info>
@author:     Damian Abalo Miron <damian.abalo@cern.ch>

@copyright:  2015 CERN, Universidad de Oviedo.

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

import errno
import logging
import json
import sys
import os
import platform
import hashlib
import shutil  # for copying files, etc
import glob
from colorama import Fore, Style
from quasar_basic_utils import yes_or_no
import version_control_interface
from DesignInspector import DesignInspector
from transformDesign import run_indent_tool
from version_control_interface import VersionControlInterface

ask=False



class File():
    '''Represents the File entry from files.txt or original_files.txt

    Fields:
    specification - is a dictionary that we got from reading the JSON representation of this object.
    This remains the original specification, i.e. use_defaults is not expanded.
    path - absolute path to where the file is.
    must_exist - True if file needs to exist in a valid quasar project.
    must_be_versioned - True if file needs to be under VCS in a valid quasar project.

    '''

    # flags are booleans that are always accessible within File object. If respective keys are absent
    # In the JSON repr then they evaluate to being false.
    Flags = ['must_exist', 'must_be_versioned', 'deprecated']

    # Attrs are like flags besides no value evalutes to None. They normally are not booleans but enums
    # or lists or something else.
    Attrs = ['md5', 'use_defaults', 'install']


    def __init__(self, base_name, specification, directory, directory_spec):
        """Makes a File entry

        Arguments:
        base_name - a string with the name of the file, like 'BaseQuasarServer.cpp'
        specification - a dictionary with keys from the set allowed_keys
        directory - absolute path to the directory which contains the file
        directory_spec - a dictionary with keys describing the directory. Used if inheriting file_defaults.
        """

        logging.debug(f"Processing File entry, base_name is {base_name}")
        self.name = base_name
        self.path = os.path.join(directory, base_name)
        self.specification = specification
        input_specification = directory_spec['file_defaults'] if 'use_defaults' in specification else {}
        input_specification.update(specification)
        for flag in File.Flags:
            flag_val = input_specification.get(flag, False)
            self.__setattr__(flag, flag_val)
        for attr in File.Attrs:
            attr_val = input_specification.get(attr, None)
            self.__setattr__(attr, attr_val)

    def must_be_md5_checked(self):
        return self.md5 is not None

    def install_action(self):
        '''Returns the x from install=x entry that governs how the installer should behave, None if
           field missing.'''
        return self.install

    @staticmethod
    def compute_md5(file_name):
        # Piotr: used the nice recipe from https://stackoverflow.com/questions/3431825/generating-an-md5-checksum-of-a-file
        hash_md5 = hashlib.md5()
        with open(file_name, 'rb') as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()


    def check_md5(self):
        '''Returns a list of problems related to md5 issues, or empty list if things are OK'''
        logging.debug("---> Checking md5 of file: %s", self.path)
        if not os.path.isfile(self.path):
            return ['Cant checksum because the file doesnt exist: '+self.path]
        else:
            md5 = self.compute_md5(self.path)
            logging.debug('md5 check was performed on file f{self.name}')
            if md5 != self.md5:
                return [f'MD5 Failure at file: {self.path} md5_obtained={md5} md5_expected={self.md5}']
            else:
                return []

    def check_consistency(self, vci):
        logging.debug("--> check_consistency called on File: %s ", self.path)
        problems=[]
        if self.must_exist:
            if not os.path.isfile(self.path):
                problems.append('File must exist but it doesnt: '+self.path)
        # check versioning
        if self.must_be_versioned:
            # this applies only if file exists
            if os.path.isfile(self.path):
                logging.debug("----> checking if versioned: %s", self.path)
                if not vci.is_versioned(self.path):
                    if ask:
                        print("File is not versioned: "+self.path)
                        yn = yes_or_no("Do you want to fix that now?")
                        if yn == 'y':
                            vci.add_to_vc(self.path)
                        else:
                            problems.append('File not versioned: '+self.path)
                    else:
                        problems.append('File not versioned: '+self.path)

        if self.must_be_md5_checked():
            problems.extend(self.check_md5())
        if self.deprecated:
            if os.path.isfile(self.path):
                if ask:
                    print("File is deprecated: "+self.path)
                    yn = yes_or_no("Do you want to fix that now?")
                    if yn == 'y':
                        if vci.is_versioned(self.path):
                            print('Attempting delete with your version control system: you will have to commit afterwards!')
                            vci.remove_from_vc(self.path)
                        else:
                            print('Deleting deprecated file: '+self.path)
                            os.remove(self.path)
                            pass
                    else:
                        problems.append('This file is deprecated, please remove it: '+self.path)
                else:
                    problems.append('This file is deprecated, please remove it: '+self.path)

        return problems

    def make_md5(self):
        self.md5 = self.compute_md5(self.path)

    def json_repr(self):
        if self.use_defaults:
            return {'use_defaults':'file_defaults_of_directory', 'md5':self.md5}
        else:
            flags_dict = {flag:self.__getattribute__(flag) for flag in File.Flags if self.__getattribute__(flag)}
            attrs_dict = {attr:self.__getattribute__(attr) for attr in File.Attrs if self.__getattribute__(attr) is not None}
            overall = flags_dict
            overall.update(attrs_dict)
            return overall

# TODO subclassing dict is probably not worth stating.
class Directory(dict):
    '''Represents a 'Directory' entry in files.txt / original_files.txt'''

    allowed_keys=['install']
    def __init__(self, basename, specification, project_directory):
        """Creates the Directory entry

        Arguments:
        basename - name of directory relating to project_directory, e.g. 'bin' or 'Device/templates'
        specification - a dict to describe relevant features of the directory
        project_directory - where the project is.
        """

        logging.debug(f'Processing Directory entry, base_name is {basename}')
        self['files']=[]
        # TODO refactor this: files and name as normal class entries
        self['name']=basename
        self['install'] = specification.get('install', None)
        if basename == '.':
            dir_path = project_directory
        else:
            dir_path = os.path.join(project_directory, basename)
        for file_name in specification['files']:
            # TODO: dir path is likely totally wrong
            file = File(file_name, specification['files'][file_name], dir_path, specification)
            self.add_file(file)
        self.specification = specification

    def add_file(self,file):
        self['files'].append(file)

    def check_consistency(self, vci):
        problems=[]
        for f in self['files']:
            problems.extend(f.check_consistency(vci))
        return problems

    def install_action(self):
        '''Returns the x from install=x entry that governs how the installer should behave, None if
           field missing.'''
        return self.get('install', None)

    def json_repr(self):
        '''Returns a dictionary that represents specification of this Directory'''
        answer = {}
        if self.install_action():
            answer['install'] = self.install_action()
        files_repr = {file.name:file.json_repr() for file in self['files']}
        answer['files'] = files_repr
        if 'file_defaults' in self.specification:
            answer['file_defaults'] = self.specification['file_defaults']
            answer['file_defaults'].pop('use_defaults', None)
        return answer

def check_consistency(directories, project_directory, vci):
    # to the files loaded from files.txt we have to add files that would be generated for defined classes
    designInspector = DesignInspector(os.path.sep.join([project_directory, 'Design', 'Design.xml']))
    classes = designInspector.get_names_of_all_classes(only_with_device_logic=True)

    # Add design-derived checks such that device classes are taken into account for a check.
    directory_Device_include = [dir for dir in directories if dir['name'] == "Device/include"][0]
    directory_Device_src = [dir for dir in directories if dir['name'] == "Device/src"][0]
    device_module_path = os.path.sep.join([project_directory, "Device"])
    for klass in classes:
        spec = {'must_exist': True, 'must_be_versioned': True}
        directory_Device_include.add_file(File(f"D{klass}.h", spec, os.path.join(device_module_path, 'include'), {}))
        directory_Device_include.add_file(File(f"D{klass}.cpp", spec, os.path.join(device_module_path, 'src'), {}))

    problems=[]

    for d in directories:
        problems.extend(d.check_consistency(vci))
    return problems


def scan_dir(dir):
    files=[]
    contents = os.listdir(dir)
    for c in contents:
        full_path=dir+os.path.sep+c
        if os.path.isfile(full_path):
            files.append(full_path)
        elif os.path.isdir(full_path):
            if c!=".svn" and c!=".git":
                files.extend (scan_dir(full_path))
        else:
            print('skipped: '+full_path+' which is neither file nor directory')
    return files

def check_uncovered(directories,project_directory):
    # build a list of all covered files
    all_files=scan_dir(project_directory)
    for d in directories:
        for f in d['files']:
            if f.path in all_files:
                all_files.remove(f.path)
    print("uncovered files:")
    for f in all_files:
        print(f)

def load_file(file_name, project_directory):
    '''Loads files.txt or original_files.txt, returns a list of Directory entries'''
    logging.debug(f'Loading file {file_name}')
    files_txt = open(file_name, 'r', encoding='utf-8')
    json_repr = json.load(files_txt)
    # json_repr at this stage should be a dictionary with directory name as key and
    # a dict of dictionary options as value.
    directories = []
    for dir_name in json_repr:
        directory = Directory(dir_name, json_repr[dir_name], project_directory)
        directories.append(directory)

    return directories

def create_release(directories):
    '''Internal (quasar-devs only) function to regenerate files.txt file'''
    files_txt = open(os.path.join('FrameworkInternals', 'files.txt'), 'w', encoding='utf-8')

    for dir in directories:
        for file in dir['files']:
            if file.must_be_md5_checked():
                file.make_md5()
    json_repr = {dir['name']:dir.json_repr() for dir in directories}
    json.dump(json_repr, files_txt, indent=4, sort_keys=True)
    print('file files.txt was created')

class Installer():
    '''The thing that installs quasar projects'''

    def __init__(self, files_txt_list_of_dirs):
        '''files_txt_list_of_dirs is the structure loaded from files.txt'''
        self.files_txt_list_of_dirs = files_txt_list_of_dirs

    def __install_directory(self, dir, target_dir_path):
        if dir.install_action() == 'create':
            if not os.path.isdir(target_dir_path):
                print('Creating directory ' + target_dir_path) # refactor for logging.
                os.mkdir(target_dir_path)
        elif dir.install_action() is None:
            pass # nothing to do
        else:
            raise Exception (('The install action {0} for directory {1} seems invalid, file a '
                              'quasar bug report.').format(dir.install_action(), dir['name']))

    def __install_file(self, file, source_file_path, target_file_path):
        if file.install_action() is None:
            return # nothing to do.
        if file.install_action() == 'overwrite':
            if not os.path.isfile(target_file_path):
                print('Copying '+source_file_path+' -> '+target_file_path)
                shutil.copy2(source_file_path,  target_file_path)
            else:
                print('Overwriting: '+target_file_path)
                shutil.copy2(source_file_path,  target_file_path)
        elif file.install_action() == 'ask_to_merge':
            # if the target file doesnt exist, just copy it
            if not os.path.isfile(target_file_path):
                print('Copying '+source_file_path+' -> '+target_file_path)
                shutil.copy2(source_file_path,  target_file_path)
            else:
                # maybe the files are the same and it is not needed to merge ??
                if os.system('diff '+source_file_path+' '+target_file_path)==0:
                    print('Files the same; merging not needed')
                else:
                    print('Filed differ; merging needed')
                    merge_val=os.system('kdiff3 -o '+target_file_path+' '+source_file_path+' '+target_file_path)
                    print('Merge tool returned: '+str(merge_val))
                    if merge_val!=0:
                        yn=yes_or_no('Merge tool returned non-zero, wanna continue?')
                        if yn=='n':
                            sys.exit(1)
        elif file.install_action() == 'copy_if_not_existing':
            if not os.path.isfile(target_file_path):
                print('Copying '+source_file_path+' -> '+target_file_path)
                shutil.copy2(source_file_path,  target_file_path)
        elif file.install_action() == 'dont_touch':
            pass
        else:
            raise Exception ( 'install='+file.install_action()+' not valid' )

    def install(self, source_directory, target_directory):
        if not os.path.isdir(target_directory):
            raise Exception(('given target_directory {0} does not exist or is not '
                             'a directory').format(target_directory))
        for dir in self.files_txt_list_of_dirs:
            source_dir_path = os.path.join(source_directory, dir['name'])
            target_dir_path = os.path.join(target_directory, dir['name'])
            self.__install_directory(dir, target_dir_path)
            for file in dir['files']:
                source_file_path = os.path.join(source_dir_path, file.name)
                target_file_path = os.path.join(target_dir_path, file.name)
                logging.debug("at file %s", file.path)
                self.__install_file(file, source_file_path, target_file_path)

def project_setup_svn_ignore(project_directory):

    cmd="svn propset svn:ignore -F .gitignore -R "+project_directory
    print('Will call: '+cmd)
    os.system(cmd)



def check_file_for_mtime ( design_mtime, p, project_directory, type, c ):
    if not os.path.isfile(p):
        print('*** ERROR: Following device file doesnt exist:')
        print('  '+p)


    else:
        file_mtime = os.path.getmtime( p )
        if design_mtime > file_mtime:
            print('*** WARNING: Following device file is older than your design file:')
            print('  '+p)
            print('  If build goes bananas, this could be one of the reasons.')


#manage files API starts here
def mfCheckConsistency(param=None):
    """Checks the consistency of the project, checking that all the files that must exist do exist, everything is in svn and the md5 keys are correct."""
    vci = version_control_interface.VersionControlInterface('.')
    global svnClient

    global ask
    if param == "--ask":
        ask = True
    directories = load_file(os.path.join('FrameworkInternals', 'files.txt'), os.getcwd())
    problems=check_consistency(directories, os.getcwd(), vci)
    check_uncovered(directories,os.getcwd())
    if len(problems)>0:
        print("I've found this consistency problems (#problems="+str(len(problems))+")")
        for p in problems:
            print(p)
    else:
        print("No problems found.")

def mfCreateRelease(context):
    """Upgrades files.txt with the contents of original_files.txt. Expert command, only to be used by developers of the framework when creating a new release"""
    os.chdir(context['projectSourceDir'])
    directories = load_file(os.path.join('FrameworkInternals','original_files.txt'), os.getcwd())
    create_release(directories)

def mfInstall(sourceDirectory, targetDirectory):
    """Installs or upgrades the framework in a given directory

    Keyword arguments:
    sourceDirectory -- The directory where the framework is currently
    targetDirectory -- The target directory where the framework will be installed or upgraded
    """
    files_txt_list_of_dirs = load_file(os.path.join('FrameworkInternals', 'files.txt'), os.getcwd())
    installer = Installer(files_txt_list_of_dirs)
    installer.install(sourceDirectory, targetDirectory)

def mfSetupSvnIgnore():
    """Setups the .svnignore hidden file, so the generated files will be ignored in your svn repository."""
    project_setup_svn_ignore(os.getcwd())

def mfCheckSvnIgnore():
    """Checks that the .svnignore hidden file is properly set up to ignore the generated files in your repository."""
    check_svn_ignore_project(os.getcwd())

def copyIfNotExists(src, dst):
    if not os.path.exists( os.path.dirname(dst) ):
        print('File [{0}] copy rejected - destination directory does not exist [{1}]'.format(os.path.basename(src), os.path.dirname(dst)))
        return

    yepCopyFile = False
    if not os.path.exists(dst):
        yepCopyFile = True
    else:
        yepCopyFile = ( 'y' == yes_or_no('binary directory file [{0}] already exists, do you want to replace it with source directory file [{1}]? Contents *will* be overwritten.'.format(dst, src)) )

    if yepCopyFile:
        print('Copying source directory file [{0}] to binary directory file {1}'.format(os.path.basename(src), dst))
        shutil.copyfile(src, dst)
    else:
        print('Skipped: copying source file [{0}]: copy rejected by user'.format(os.path.basename(src)))

def symlinkIfNotExists(src, dst):
    try:
        os.symlink(src, dst)
        print('Symlinked {0} as {1}'.format(src, dst))
    except OSError as e:
        if e.errno == errno.EEXIST:
            print('Skipped {0} because: target already exists'.format(src))
        else:
            raise e

def symlinkRuntimeDeps(context, wildcard=None):
    # for windows runtime deps are copied; this avoids requiring elevated privileges (windows symlink requires admin rights)
    linkerFunction = copyIfNotExists if platform.system().lower() == 'windows' else symlinkIfNotExists
    try:
        if wildcard is None:
            yn = yes_or_no('No argument provided, will symlink ServerConfig.xml and all config*.xml files, OK?')
            if yn == 'n':
                return
            linkerFunction(
                    os.path.join(context['projectSourceDir'], 'bin', 'ServerConfig.xml'),
                    os.path.join(context['projectBinaryDir'], 'bin', 'ServerConfig.xml'))
            linkerFunction(
                    os.path.join(context['projectSourceDir'], 'bin', 'ServerConfig.xsd'),
                    os.path.join(context['projectBinaryDir'], 'bin', 'ServerConfig.xsd'))
            config_files = glob.glob(os.path.join(context['projectSourceDir'], 'bin', 'config*.xml'))
            for config_file in config_files:
                linkerFunction(config_file, os.path.join(context['projectBinaryDir'], 'bin', os.path.basename(config_file)))
        else:
            config_files = glob.glob(os.path.join(context['projectSourceDir'], 'bin', wildcard))
            print('Matched {0} files'.format(len(config_files)))
            for config_file in config_files:
                linkerFunction(config_file, os.path.join(context['projectBinaryDir'], 'bin', os.path.basename(config_file)))
    except OSError as exception:
        if exception.errno == errno.ENOENT:
            print(Fore.RED + 'You can call this only when build directory exists.'
                  + Style.RESET_ALL)
            print(Fore.GREEN + 'E.g. run it after running "quasar.py build"' + Style.RESET_ALL)
        else:
            raise

def _style_it(file_path, vcs):
    # check if file in the VCS (so, safe)
    # TODO: can put it on the quasar's VCS interface
    if vcs.file_has_uncommitted_changes(file_path):
        print(f'{file_path}: {Fore.RED}Cowardly refusing to touch a file which is new or modified'
            f'{Style.RESET_ALL} (i.e. not committed), because you won\'t have any backup')
        return
    # perform styling
    with open(file_path, 'rb') as file:
        unindented_content = file.read()
    with open(file_path, 'wb') as file:
        run_indent_tool(unindented_content, file)
    if vcs.file_has_uncommitted_changes(file_path):
        print (f'{file_path}: {Fore.GREEN}Style fixed, commit if happy.{Style.RESET_ALL}')
    else:
        print (f'{file_path}: {Fore.BLUE}Nothing changed{Style.RESET_ALL}')

def command_style_it(context, *args):
    '''Will run source code styler for listed files (or --device)'''
    vcs = VersionControlInterface('.') # need it to only perform it on committed files for safety
    for item in args:
        if item[0] != '-': # no hyphen at the beginning of the command.
            _style_it(item, vcs)
        else:
            if item == '--device':
                for header_file in glob.glob('Device/include/*.h'):
                    _style_it(header_file, vcs)
                for body_file in glob.glob('Device/src/*.cpp'):
                    _style_it(body_file, vcs)
            else:
                print('Unable to interprete command options')
