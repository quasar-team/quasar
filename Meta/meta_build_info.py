import os
import socket
import sys
import argparse
from argparse import RawTextHelpFormatter
from time import localtime, strftime

def process_args():
	print 'meta_build_info.py, processing command line args...'
	parser = argparse.ArgumentParser(description="""meta_build_info.py generates build info. Requires 3 arg - the ABS path to the target generation directory (must already exist)""", formatter_class=RawTextHelpFormatter)
	parser.add_argument('--target_generation_dir', required=True, help='absolute path to local directory where the build meta info will be generated')
	parser.add_argument('--commit_ID', required=True, help='current git/SVN commit ID for the repository')
	parser.add_argument('--toolkit_libs', required=True, help='which opc-ua toolkit libraries was this server built with')
	args = parser.parse_args()
	print 'generating to directory [{0}], commitID [{1}] toolkit libs [{2}]'.format(args.target_generation_dir, args.commit_ID, args.toolkit_libs)
	return args

def open_generation_target_file(target_generation_dir):
	if not os.path.exists(target_generation_dir):
		sys.exit('Generation Failed! Could not find target generation dir [{0}]'.format(target_generation_dir))
	generation_file_path = os.path.join(target_generation_dir,  'MetaBuildInfoGenerated.h')
	try:
		return open(generation_file_path, 'w')
	except Exception, e:
		sys.exit('Generation Failed! Could not open/create target generation file [{0}]'.format(generation_file_path))

def generateBuildTime(generatedFile):
	now = strftime("%a, %d %b %Y %H:%M:%S", localtime())
	generatedFile.write('#define GEND_BUILD_TIME "{0}"\n'.format(now))

def generateBuildHost(generatedFile):
	build_host = socket.gethostname()
	generatedFile.write('#define GEND_BUILD_HOST "{0}"\n'.format(build_host))

def generateCommitID(generatedFile, commitID):
	generatedFile.write('#define GEND_COMMIT_ID "{0}"\n'.format(commitID))

def generateToolkitLibs(generatedFile, toolkitLibs):
	strippedLibs = toolkitLibs.replace("\\", "")
	generatedFile.write('#define GEND_TOOLKIT_LIBS "{0}"\n'.format(strippedLibs))

args = process_args()
generation_file = open_generation_target_file(args.target_generation_dir)
generateBuildTime(generation_file)
generateBuildHost(generation_file)
generateCommitID(generation_file, args.commit_ID)
generateToolkitLibs(generation_file, args.toolkit_libs)
generation_file.close()
print 'generated build meta info to file [{0}]'.format(generation_file)