#!/bin/csh
# git push quasar branch arrays4 quasar-team gitbub, only the clean fwk
git status
find ./ -name "CMakeCache.txt" -exec rm {} \;
find ./ -name "CMakeFiles" -exec rm -rf {} \;
rm -rf CMakeFiles
rm -rf html
rm -rf latex
# come up with a message for git
set MSG1=`date`" eclipse-push from "`whoami`"@"`hostname`
echo ${MSG1}
rm -f ./tmp_msg.txt
echo ${MSG1}"\n" > ./tmp_msg.txt
gedit ./tmp_msg.txt
set MSG=`cat ./tmp_msg.txt`

git add --all
git commit -m "${MSG}"
git push origin arrays4



