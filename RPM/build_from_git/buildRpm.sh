#!/bin/bash

# A script that setups ground for rpmbuild program.
# It checks out from git, adapts specfile with version information, adjusts ServerConfig.xml version information etc and then runs RPM builder
# Author: Piotr Nikiel, CERN

rm -Rf checkout


REPO_PATH=github.com/.....   # Here you put your repo path


VERSION_STR=  # Leave this empty

if [ $# -ne 1 ]; then
	echo "Usage: "
	echo "./buildRpm.sh <commit_id/tag/branch_id>"
	exit -1

fi
RPM_VERSION=$1
RPM_RELEASE=0
VERSION_ADDITIONAL="\nBuilt by: $USER on `date` on $HOSTNAME arch=`arch` "
VERSION_STR="$VERSION_STR $VERSION_ADDITIONAL"

git clone  $REPO_PATH checkout
cd checkout
git checkout $1
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

