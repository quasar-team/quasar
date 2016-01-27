#!/bin/sh

# First ---- check if everything is commited

TAG=$1
if [ "$TAG" == "" ]; then
  echo "Please provide intended TAG version , without project name e.g. 100.200.300 "
  exit 1
fi

if [ "`svn stat ../ |wc -l`" != "0" ]; then   
  echo "Some files are probably not commited, fix that first"
  exit 1 
fi

# after this put a version info into files
echo $TAG > ../Design/genericFrameworkVersion.txt
echo "#define QUASAR_VERSION_STR \"$TAG\"" > ../Server/include/QUASARFrameworkVersion.h

python ../quasar.py create_release

# after this files.txt and genericFrameworkVersion.txt has changed so should be recommited
svn commit -m "files.txt fix for tag=$1" ../

svn cp https://svn.cern.ch/reps/atlasdcs/OpcUaGenericServer/trunk https://svn.cern.ch/reps/atlasdcs/OpcUaGenericServer/tags/OpcUaGenericServer-$1

