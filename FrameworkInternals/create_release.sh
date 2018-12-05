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
echo $TAG > ../Design/quasarVersion.txt || exit 1
echo "#define QUASAR_VERSION_STR \"$TAG\"" > ../Server/include/QuasarVersion.h || exit 1

python ../quasar.py create_release || exit 1

# after this files.txt and genericFrameworkVersion.txt has changed so should be recommited
git commit -m "files.txt update in preparation for tag=$1" ../ || exit 1

git push origin master || exit 1

git tag -a "v$1" || exit 1

git push origin "v$1" || exit 1
