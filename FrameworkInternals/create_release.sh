#!/bin/sh

# First ---- check if everything is commited

TAG=$1
if [ "$TAG" == "" ]; then
  echo "Please provide intended TAG version , without project name e.g. 100.200.300 "
  exit 1
fi



if [ "`git status --porcelain ../ |wc -l`" != "0" ]; then   
    echo "Some files are probably not commited, fix that first"
    echo "To see the list run git status"
  exit 1 
fi

# after this put a version info into files
echo $TAG > ../Design/genericFrameworkVersion.txt
echo "#define QUASAR_VERSION_STR \"$TAG\"" > ../Server/include/QUASARFrameworkVersion.h

python ../quasar.py create_release

# after this files.txt and genericFrameworkVersion.txt has changed so should be recommited
git commit -m "files.txt update in preparation for tag=$1" ../

git push origin master

git tag -a "v$1"

git push origin "v$1"
