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
        self.project_path = project_path
        magic_found_per_type = {}
        for vcs_type in known_vcs_types:
            magic_found_per_type[vcs_type] = os.path.isdir(self.project_path + os.path.sep + magic_vcs_dir[vcs_type])
        #  see what vcs types were identified: preferably there should be exactly one
        magic_found = [x for x in magic_found_per_type if magic_found_per_type[x]]
        if len(magic_found) < 1:
            raise Exception ('Project is unversioned or it was impossible to determine the version control system used')
        elif len(magic_found) > 1:
            found_str = ','.join(magic_found)
            raise Exception ('Project seems versioned under multiple version control systems. Please fix first. [Hint: found these vcs:'+found_str+']')
        self.vcs_type = magic_found[0]
        print('Determined vcs type: '+self.vcs_type)

        try:
            if self.vcs_type == 'git':
                import pygit2
                self.repo = pygit2.Repository(self.project_path)
            elif self.vcs_type == 'svn':
                import pysvn
                self.svnClient = pysvn.Client()
            else:
                raise Exception('Internal quasar error')
        except Exception as e:
            raise Exception('It was impossible to import Python module for your version control system type. Original exception:'+str(e))


    def is_versioned(self,file_path):
        if self.vcs_type == 'git':
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            index = self.repo.index
            index.read()
            try:
                index[file_path]
            except:
                return False
            return True
        elif self.vcs_type == 'svn':
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
        if self.vcs_type == 'git':
            #  file path should start at the base
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            self.repo.index.read()
            self.repo.index.add(file_path)
            self.repo.index.write()
        elif self.vcs_type == 'svn':
            self.svnClient.add(file_path)
        else:
            raise Exception('Internal quasar error')

    def remove_from_vc(self,file_path):
        if self.vcs_type == 'git':
            #  file path should start at the base
            file_path = file_path.replace(os.getcwd()+os.path.sep,'')
            self.repo.index.read()
            self.repo.index.remove(file_path)
            self.repo.index.write()
        elif self.vcs_type == 'svn':
            self.svnClient.remove(file_path)
        else:
            raise Exception('Internal quasar error')

    def get_latest_repo_commit(self):
        commitID = "Failed to find commitID"
        try:
            if self.vcs_type == 'git':
                import pygit2
                commitID = self.repo.describe(describe_strategy=pygit2.GIT_DESCRIBE_ALL, show_commit_oid_as_fallback=True, always_use_long_format=True, dirty_suffix='-dirty')
            elif self.vcs_type == 'svn':
                commitID = self.svnClient.info2(self.project_path)[0][1]['rev'].number
        except Exception as e:
            commitID = 'Exception: {}'.format(str(e))
        return 'VCS type [{}] commit ID [{}]'.format(self.vcs_type, commitID)

    def file_has_uncommitted_changes(self, file_path):
        '''Returns True if file is entirely committed (i.e. it can be reverted just in case)'''
        if file_path[0] == os.path.sep:
            raise NotImplementedError("not impl for absolute paths")
        if self.vcs_type == 'git':
            return file_path in self.repo.status()
        else:
            raise NotImplementedError("not impl for other backends")
