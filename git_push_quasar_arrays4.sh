#!/bin/csh
# git push quasar branch arrays4 quasar-team gitbub, only the clean fwk
# git status
find ./ -name "CMakeCache.txt" -exec rm {} \;
find ./ -name "CMakeFiles" -exec rm -rf {} \;
rm -rf CMakeFiles
rm -rf html
rm -rf latex
# come up with a message for git
set MSG=`date`" eclipse-push from "`whoami`"@"`hostname`
echo ${MSG}
git add --all
git commit -m "${MSG}"
git push origin arrays4



