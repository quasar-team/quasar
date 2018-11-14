import os
import socket
import sys
import argparse
import version_control_interface
from argparse import RawTextHelpFormatter
from time import localtime, strftime

def process_args():
	print 'meta_build_info.py, processing command line args...'
	parser = argparse.ArgumentParser(description="""meta_build_info.py generates build info. Requires 3 args - Abs path to project root, abs path to the target generation directory (must already exist) and the toolkit libs""", formatter_class=RawTextHelpFormatter)
	parser.add_argument('--project_root_dir', required=True, help='absolute path to quasar project root directory')
	parser.add_argument('--target_generation_dir', required=True, help='absolute path to local directory where the build meta info will be generated')
	parser.add_argument('--toolkit_libs', required=True, help='which opc-ua toolkit libraries was this server built with')
	args = parser.parse_args()
	print 'project root directory [{}] generating to directory [{}], toolkit libs [{}]'.format(args.project_root_dir, args.target_generation_dir, args.toolkit_libs)
	return args

def open_generation_target_file(target_generation_dir):
	if not os.path.exists(target_generation_dir):
		sys.exit('Generation Failed! Could not find target generation dir [{}]'.format(target_generation_dir))
	generation_file_path = os.path.join(target_generation_dir,  'MetaBuildInfoGenerated.h')
	try:
		return open(generation_file_path, 'w')
	except Exception, e:
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
		print 'generateCommitID failed, exception: {}'.format(str(e))
	generatedFile.write('#define GEND_COMMIT_ID "{}"\n'.format(commitID))

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
print 'generated build meta info to file [{0}]'.format(generation_file)
