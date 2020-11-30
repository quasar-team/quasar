'''
meta_build_info.py
Generates a header file of build time constants (build host, timestamp etc) for publication via
the quasar server StandardMetaData object

@author: Ben Farnham

@copyright:  2018 CERN

@license:
Copyright (c) 2018, CERN
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

@contact:    quasar-developers@cern.ch
'''
import os
import socket
import sys
import argparse
import version_control_interface
from argparse import RawTextHelpFormatter
from time import localtime, strftime

def process_args():
    print('meta_build_info.py, processing command line args...')
    parser = argparse.ArgumentParser(description="""meta_build_info.py generates build info. Requires 3 args - Abs path to project root, abs path to the target generation directory (must already exist) and the toolkit libs""", formatter_class=RawTextHelpFormatter)
    parser.add_argument('--project_root_dir', required=True, help='absolute path to quasar project root directory')
    parser.add_argument('--target_generation_dir', required=True, help='absolute path to local directory where the build meta info will be generated')
    parser.add_argument('--toolkit_libs', required=True, help='which opc-ua toolkit libraries was this server built with')
    args = parser.parse_args()
    print('project root directory [{}] generating to directory [{}], toolkit libs [{}]'.format(args.project_root_dir, args.target_generation_dir, args.toolkit_libs))
    return args

def open_generation_target_file(target_generation_dir):
    if not os.path.exists(target_generation_dir):
        sys.exit('Generation Failed! Could not find target generation dir [{}]'.format(target_generation_dir))
    generation_file_path = os.path.join(target_generation_dir,  'MetaBuildInfoGenerated.h')
    try:
        return open(generation_file_path, 'w', encoding='utf-8')
    except Exception as e:
        sys.exit('Generation Failed! Could not open/create target generation file [{}]'.format(generation_file_path))

def generateBuildTime(generatedFile):
    now = strftime("%a, %d %b %Y %H:%M:%S", localtime())
    generatedFile.write('#define GEND_BUILD_TIME "{}"\n'.format(now))

def generateBuildHost(generatedFile):
    build_host = socket.gethostname()
    generatedFile.write('#define GEND_BUILD_HOST "{}"\n'.format(build_host))

def generateCommitID(generatedFile, project_root_dir):
    commitID = "No commit ID found"
    try:
        commitID = version_control_interface.VersionControlInterface(project_root_dir).get_latest_repo_commit()
    except Exception as e:
        print('generateCommitID failed, exception: {}'.format(str(e)))
    generatedFile.write('#define GEND_COMMIT_ID "{}"\n'.format(commitID.replace('"', '\\"')))

def generateToolkitLibs(generatedFile, toolkitLibs):
    strippedLibs = toolkitLibs.replace("\\", "")
    generatedFile.write('#define GEND_TOOLKIT_LIBS "{}"\n'.format(strippedLibs))

args = process_args()
generation_file = open_generation_target_file(args.target_generation_dir)
generateBuildTime(generation_file)
generateBuildHost(generation_file)
generateCommitID(generation_file, args.project_root_dir)
generateToolkitLibs(generation_file, args.toolkit_libs)
generation_file.close()
print('generated build meta info to file [{0}]'.format(generation_file))
