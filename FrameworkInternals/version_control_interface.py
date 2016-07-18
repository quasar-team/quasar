'''
version_control_interface.py

This serves as the unified access to supported version control systems (atm Git and SVN).

@author:     Piotr Nikiel <piotr@nikiel.info>

@copyright:  2015 CERN

@license:
Copyright (c) 2015, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    piotr.nikiel@cern.ch

@todo: (piotr) One day we may consider real polymorphism for various vcs implementation. At the moment it is rather if/else.

'''

import os

known_vcs_types = ['git', 'svn']
magic_vcs_dir = {'git':'.git', 'svn':'.svn'}

class VersionControlInterface:
    def __init__(self,project_path):
        magic_found_per_type = {}
        for vcs_type in known_vcs_types:
            magic_found_per_type[vcs_type] = os.path.isdir(project_path + os.path.sep + magic_vcs_dir[vcs_type])
        #  see what vcs types were identified: preferably there should be exactly one
        magic_found = filter( lambda x: magic_found_per_type[x], magic_found_per_type )
        if len(magic_found) < 1:
            raise Exception ('Project is unversioned or it was impossible to determine the version control system used')
        elif len(magic_found) > 1:
            found_str = ','.join(magic_found)
            raise Exception ('Project seems versioned under multiple version control systems. Please fix first. [Hint: found these vcs:'+found_str+']')
        self.vcs_type = magic_found[0]
        print 'Determined vcs type: '+self.vcs_type
        
        try:
            if self.vcs_type is 'git':
                import pygit2
                self.repo = pygit2.Repository(project_path)
            elif self.vcs_type is 'svn':
                import pysvn
                self.svnClient = pysvn.Client()
            else:
                raise Exception('Internal quasar error')
        except Exception as e:
            raise Exception('It was impossible to import Python module for your version control system type. Original exception:'+str(e))
        
    
    def is_versioned(self,file_path):
        if self.vcs_type is 'git':
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            index = self.repo.index
            index.read()
            try:
                index[file_path]
            except:
                return False
            return True
        elif self.vcs_type is 'svn':
            try:
                statuses = self.svnClient.status(file_path)
            except:
                return False
            if len(statuses)==0:
                return False
            elif len(statuses)!=1:
                raise Exception ("bizaarre: len(statuses)="+str(len(statuses)))
            status = statuses[0]
            return status['is_versioned']==1
        else:
            raise Exception('Internal quasar error')
        
    def add_to_vc(self,file_path):
        if self.vcs_type is 'git':
            #  file path should start at the base
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            self.repo.index.read()
            self.repo.index.add(file_path)
            self.repo.index.write()
        elif self.vcs_type is 'svn':
            self.svnClient.add(file_path)
        else:
            raise Exception('Internal quasar error')
               
    def remove_from_vc(self,file_path):
        if self.vcs_type is 'git':
            #  file path should start at the base
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            self.repo.index.read()
            self.repo.index.remove(file_path)
            self.repo.index.write()
        elif self.vcs_type is 'svn':
            self.svnClient.remove(file_path)
        else:
            raise Exception('Internal quasar error')         
