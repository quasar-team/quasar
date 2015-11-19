#!/bin/bash

# A script that setups ground for rpmbuild program.
# It downloads proper checkout from the SVN, adapts specfile with version information, adjusts ServerConfig.xml version information etc and then runs RPM builder
# Author: Piotr Nikiel, ATLAS Central DCS, CERN

rm -Rf checkout

EXPORT_SVN_PATH=
VERSION_STR=

if [ $# -ne 2 ]; then
	echo "Usage: "
	echo "./buildRpm.sh --tag <tag name only numbers eg 1.2.3-4 >"
	echo "or"
	echo "./buildRpm.sh --revision <SVN revision number of trunk>"
	exit -1
elif [ $1 = "--tag" ]; then
	echo "Building from tag: $2"
	TAG=$2
	TAG_ARRAY=(${TAG//-/ })
	RPM_VERSION=${TAG_ARRAY[0]}
	RPM_RELEASE=${TAG_ARRAY[1]}

	echo "PLEASE setup SVN path to your repository in buildRpm.sh file"
	exit 1
	# Example: you should setup a path like that (this is an example from OpcUaCanOpenServer):
	# This is the path to tag:
	# EXPORT_SVN_PATH=https://svn.cern.ch/reps/atlasdcs/OpcUaElmb/tags/OpcUaCanOpenServer-$RPM_VERSION-$RPM_RELEASE/ 

	VERSION_STR="Built from tag: $TAG"
	svn export $EXPORT_SVN_PATH checkout
	if [ $? -ne 0 ]; then
		exit -1
	fi
elif [ $1 = "--revision" ]; then
	echo "Building from revision: $2"


	echo "PLEASE setup SVN path to your repository in buildRpm.sh file"
	exit 1
	# Example: you should setup a path like that (this is an example from OpcUaCanOpenServer):
	# This is the path to trunk:
	# EXPORT_SVN_PATH=https://svn.cern.ch/reps/atlasdcs/OpcUaElmb/trunk/OpcUaCanOpen

	VERSION_STR="Built from revision: $2"
	svn export $EXPORT_SVN_PATH -r $2 checkout
	if [ $? -ne 0 ]; then
		exit -1	
	fi
	RPM_VERSION=r$2.0.0
	RPM_RELEASE=0
else
	echo "--tag or --revision only allowed"
fi
VERSION_ADDITIONAL="\nBuilt by: $USER on `date` on $HOSTNAME arch=`arch` "
VERSION_STR="$VERSION_STR $VERSION_ADDITIONAL"

echo "#define VERSION_STR \"$VERSION_STR\"" > checkout/Server/src/version.h
sed "s/<SoftwareVersion>[0-9.a-zA-Z]*<\/SoftwareVersion>/<SoftwareVersion>$VERSION_STR<\/SoftwareVersion>/ "  checkout/bin/ServerConfig.xml > checkout/bin/ServerConfig.xml.new
rm checkout/bin/ServerConfig.xml
mv checkout/bin/ServerConfig.xml.new checkout/bin/ServerConfig.xml
rm -f rpmbuild/SOURCES/checkout.tar.gz
rm -f rpmbuild/SPECS/spec.spec
rm -Rf rpmbuild/BUILD/*
rm -Rf rpmbuild/BUILDROOT/*
tar cf rpmbuild/SOURCES/checkout.tar checkout/
gzip -1 rpmbuild/SOURCES/checkout.tar 
echo "%define version $RPM_VERSION" > rpmbuild/SPECS/spec.spec
echo "%define release $RPM_RELEASE" >> rpmbuild/SPECS/spec.spec
cat checkout/RPM/build_from_svn/template.spec >> rpmbuild/SPECS/spec.spec
rm -Rf checkout
cd rpmbuild
rpmbuild -bb SPECS/spec.spec
if [ $? -eq 0 ]; then
	echo "rpmbuild finished OK, your rpm should be here: `pwd`/rpmbuild/RPMS/x86_64/"
else
	echo "rpmbuild finished with error code, sorry, no RPM"
fi

