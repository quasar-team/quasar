#!/bin/bash

# A script that setups ground for rpmbuild program.
# It checks out from git, adapts specfile with version information, adjusts ServerConfig.xml version information etc and then runs RPM builder
# Author: Piotr Nikiel, CERN

rm -Rf checkout

VERSION_STR="Built by quasar RPM builder for git, args given: $1 $2"

REPO_PATH=github.com/.....   # Here you put your repo path

if [ $# -ne 2 ]; then
	echo "Usage: "
	echo "./buildRpm.sh --tag <tag name only numbers eg 1.2.3-4 >"
	echo "or"
	echo "./buildRpm.sh --branch <git branch>"
	echo "Note: building from branches is only for development, because it is not uniquely identifiable. For distribution and production always build from tag."
	exit -1
elif [ $1 = "--tag" ]; then
	echo "Building from tag: $2"
	TAG=$2
	TAG_ARRAY=(${TAG//-/ })
	RPM_VERSION=${TAG_ARRAY[0]}
	RPM_RELEASE=${TAG_ARRAY[1]}
elif [ $1 = "--branch" ]; then
        echo "Building from branch: $2"
    	RPM_VERSION=$( echo "$2" | tr - _)  # replace dashes with undescrores (dashes are illegal in RPM version numbers)
	RPM_RELEASE=0
else
	echo "--tag or --branch only allowed"
fi

if [ -z "$RPM_RELEASE" ]; then
    RPM_RELEASE="0"
fi 

VERSION_ADDITIONAL="\nBuilt by: $USER on `date` on $HOSTNAME arch=`arch` "
VERSION_STR="$VERSION_STR $VERSION_ADDITIONAL"

git clone $REPO_PATH --depth 1 -b $2  checkout
cd checkout
if [ $? -ne 0 ]; then
  exit 1
fi
cd ..
echo "Cloned and checked out, now adapting version strings etc"
echo "#define VERSION_STR \"$VERSION_STR\"" > checkout/Server/src/version.h
sed "s/<SoftwareVersion>[0-9.a-zA-Z]*<\/SoftwareVersion>/<SoftwareVersion>$VERSION_STR<\/SoftwareVersion>/ "  checkout/bin/ServerConfig.xml > checkout/bin/ServerConfig.xml.new
rm checkout/bin/ServerConfig.xml
mv checkout/bin/ServerConfig.xml.new checkout/bin/ServerConfig.xml
rm -Rf rpmbuild
mkdir rpmbuild rpmbuild/SOURCES rpmbuild/SPECS rpmbuild/BUILD rpmbuild/BUILDROOT rpmbuild/RPMS
tar cf rpmbuild/SOURCES/checkout.tar checkout/
gzip -1 rpmbuild/SOURCES/checkout.tar 
echo "%define version $RPM_VERSION" > rpmbuild/SPECS/spec.spec
echo "%define release $RPM_RELEASE" >> rpmbuild/SPECS/spec.spec
cat checkout/RPM/build_from_git/template.spec >> rpmbuild/SPECS/spec.spec
rm -Rf checkout
cd rpmbuild
rpmbuild -bb SPECS/spec.spec
if [ $? -eq 0 ]; then
	echo "rpmbuild finished OK, your rpm should be here: `pwd`/rpmbuild/RPMS/x86_64/"
else
	echo "rpmbuild finished with error code, sorry, no RPM"
fi

