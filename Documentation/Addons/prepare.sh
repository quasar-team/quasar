#!/bin/bash

export CC="/sw/opt/swtools/x-tools7h/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-gcc"
export CXX="/sw/opt/swtools/x-tools7h/arm-unknown-linux-gnueabihf/bin/arm-unknown-linux-gnueabihf-g++"
export ORIGDIR=`pwd`

prep_libxml2()
{
 cd $ORIGDIR
 LIBXML2_NAME="libxml2-2.7.2"
 LIBXML2_FULLNAME="$LIBXML2_NAME"".tar.gz"
 LIBXML2_URL="ftp://xmlsoft.org/libxml2/$LIBXML2_FULLNAME"
 cd downloaded
 wget $LIBXML2_URL
 tar xf $LIBXML2_FULLNAME
 cd $LIBXML2_NAME
 ./configure --host=arm --disable-shared --with-c14n=no --with-catalog=no --with-docbook=no --with-ftp=no --with-html=yes --with-http=no --with-iconv=no --with-iso8859x=no --with-legacy=no --with-output=no --with-pattern=yes --with-push=no  --with-python=no --with-reader=no --with-regexps=no --with-sax1=no --with-schematron=no --with-tree=no --with-xpath=no --with-xinclude=no --with-xptr=no --with-valid=no --with-modules=no --prefix=$ORIGDIR/target 
 make && make install

 
}

prep_openssl()
{
 cd $ORIGDIR
 OPENSSL_NAME="openssl-1.0.0s"
 OPENSSL_FULLNAME="$OPENSSL_NAME"".tar.gz"
 OPENSSL_URL="http://openssl.org/source/$OPENSSL_FULLNAME"
 cd downloaded
 wget $OPENSSL_URL
 tar xf $OPENSSL_FULLNAME
 cd $OPENSSL_NAME
 ./Configure --prefix=$ORIGDIR/target linux-generic32
 make && make install
 }

prep_xercesc()
{
  cd $ORIGDIR
  XERCESC_NAME="xerces-c-3.0.1"
  XERCESC_FULLNAME="$XERCESC_NAME"".tar.gz"
  XERCESC_URL="http://archive.apache.org/dist/xerces/c/3/sources/$XERCESC_FULLNAME"
  cd downloaded
  wget $XERCESC_URL
  tar xf $XERCESC_FULLNAME
  cd $XERCESC_NAME
  ./configure --prefix=$ORIGDIR/target --host=arm --enable-shared=no --disable-network
  make && make install
}

prep_boost()
{
  cd $ORIGDIR
  BOOST_NAME=boost_1_59_0
  BOOST_FULLNAME="$BOOST_NAME".tar.bz2
  BOOST_URL="http://downloads.sourceforge.net/project/boost/boost/1.59.0/$BOOST_FULLNAME"
  cd downloaded
  wget $BOOST_URL
  tar xf $BOOST_FULLNAME
  cd $BOOST_NAME
  #bash
  
  # build Boost.Build
  cd tools/build
  ./bootstrap.sh
  mkdir BoostBuildBin
  ./b2 install --prefix=BoostBuildBin
  export PATH=`pwd`/BoostBuildBin/bin:$PATH
  cd ../../
  echo "using gcc : arm : $CC ;" > user-config.jam
  export BOOST_BUILD_PATH=`pwd`
  b2 toolset=gcc-arm --with-program_options --with-system link=static threading=multi --prefix=$ORIGDIR/target install 
  

}

prep_xsd()
{
  cd $ORIGDIR
  XSD_NAME="xsd-3.3.0-2"
  XSD_FULLNAME="$XSD_NAME".tar.bz2
  XSD_URL="http://www.codesynthesis.com/download/xsd/3.3/$XSD_FULLNAME"
  cd downloaded
  wget $XSD_URL
  tar xf $XSD_FULLNAME
  cd $XSD_NAME
  mkdir $ORIGDIR/target
  mkdir $ORIGDIR/target/include
  cp -Rv libxsd/xsd/ $ORIGDIR/target/include/
}

rm -rf downloaded
rm -rf target
mkdir downloaded
mkdir target
prep_libxml2
prep_openssl
prep_xercesc
prep_boost
prep_xsd
