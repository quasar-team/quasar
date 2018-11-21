rm -Rf amalgamate
git clone https://github.com/edlund/amalgamate.git || exit 1
rm -Rf muparser-cloned
git clone https://github.com/beltoforion/muparser.git -b v2.2.6.1 --depth=1 muparser-cloned || exit 1
ln -s `readlink -f muparser-cloned` amalgamate/muparser || exit 1
ln -s `readlink -f amalgamate_header.json` amalgamate || exit 1
ln -s `readlink -f amalgamate_source.json` amalgamate || exit 1
cat muparser-cloned/src/*.cpp > muparser-amalgamated/src/muParser.cpp
sed -e "s/#include \"muParser.*/\/\/amalgamated header include by Piotrs script \0/" -i muparser-amalgamated/src/muParser.cpp
sed -i '1i #include <muParser.h>' muparser-amalgamated/src/muParser.cpp
cd amalgamate
python amalgamate.py -c amalgamate_header.json -s muparser/include -v yes
cd ..
rm -Rf amalgamate
rm -Rf muparser-cloned


