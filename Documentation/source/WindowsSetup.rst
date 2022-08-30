Windows Setup
=============

| The QUASAR Framework has many dependencies that are common place in a
  Linux environment, however this is not the case for a Windows one. In
  this page there are provided instructions on how to install the
  different dependencies of the framework, and some useful links as
  well, to help the user find the required programs. The availability of
  the links is not guaranteed at all, as they are external links that
  may become outdated.

Compulsory Dependencies
-----------------------

| This dependencies need to be taking care of in order to be able to use
  the main functionality of the framework.

CMake

-  `Download and install from the official
   web-page <https://cmake.org/download/>`__

-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable

-  

   -  To make sure everything went well run the command *cmake* in a
      Windows Command Prompt. If the command is recognized you are good
      to go.

Python

-  `Download and install from the official
   web-page <https://www.python.org/downloads/>`__

-  

   -  Scripts were developed with Python 2.7, but this doesn't mean that
      other versions will not work

-  Add the main installation folder to the Windows PATH environment
   variable. This may be done automatically by the installer.

-  

   -  To make sure everything went well run the command *python --help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

lxml

-  `Download and install from the Python Software Foundation
   web-page <https://pypi.python.org/pypi/lxml/3.4.4>`__

-  

   -  Be careful, as you need to take into account the python version in
      order to install correctly this package.

pysvn

-  Download and install. `Working binaries found
   here <http://pysvn.tigris.org/servlets/ProjectDocumentList?folderID=1768>`__

-  

   -  Be careful, as you need to take into account the svn and the
      python version in order to install correctly this package.

Artistic Style

-  `Download and install from
   sourceforge <http://sourceforge.net/projects/astyle/files/>`__

-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable

-  

   -  To make sure everything went well run the command *astyle --help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

KDiff3

-  `Download and install from
   sourceforge <http://sourceforge.net/projects/kdiff3/files/>`__

-  Add the main installation folder to the Windows PATH environment
   variable

-  

   -  To make sure everything went well run the command *kdiff3* in a
      Windows Command Prompt. If the command is recognized you are good
      to go.

Visual Studio

-  Install visual studio, version of your choice.

-  

   -  For CERN users it is advised to do it trough CMF

-  Add the folder containing the executable MSBUILD.exe to the Windows
   PATH environment variable

-  

   -  The location of this file changes depending on the version you are
      installing. Check for help online if you can't locate it
   -  To make sure everything went well run the command *msbuild /help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

libxml2

-  `Download and install from the official
   web-page <http://www.xmlsoft.org/downloads.html>`__

-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable

-  

   -  To make sure everything went well run the command *xmllint --help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

CodeSynthesis XSD (a.k.a. xsdcxx)

-  `Download and install from the official
   web-page <http://www.codesynthesis.com/products/xsd/download.xhtml>`__
-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable
-  `Conflict with Microsoft
   xsd.exe <http://wiki.codesynthesis.com/Using_XSD_with_Microsoft_Visual_Studio>`__

   -  Microsoft SDK contains an executable with the name xsd.exe which
      may take precedence over CodeSynthesis xsd.exe. If this happens
      you will get an error message like this: Project : error PRJ0019:
      A tool returned an error code from "xsd schema.xsd. To solve this
      go into the codesynthesys xsd binary directory and rename xsd.exe
      to xsdcxx.exe. **The python scripts are expecing that you do this
      change, so it will not work unless you do it.**

-  To make sure everything went well run the command *xsdcxx help* in a
   Windows Command Prompt. If the command is recognized you are good to
   go.

Boost

-  `Download and install from
   sourceforge <http://sourceforge.net/projects/boost/files/boost-binaries/>`__

OpenSSL

-  `Download and install from Shining Light
   Productions <http://slproweb.com/products/Win32OpenSSL.html>`__

xerces-c

-  `Download from the official
   web-page <http://xerces.apache.org/xerces-c/download.cgi>`__
-  Since there is no precompiled binary available, the user will need to
   compile this software `following this
   instructions <https://xerces.apache.org/xerces-c/build-3.html#Windows>`__

Optional Dependencies
---------------------

| This dependencies are needed to access some functionality of the
  framework that it's not critical.

Graphviz

-  Needed for generating graphs

-  `Download and install from the official
   web-page <http://www.graphviz.org/>`__

-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable. This may be done automatically by
   the installer

-  

   -  To make sure everything went well run the command *dot --help* in
      a Windows Command Prompt. If the command is recognized you are
      good to go.

Doxygen

-  Needed for doxygen documentation generation

-  `Download and install from the official
   web-page <http://www.stack.nl/~dimitri/doxygen/download.html>`__

-  Add the folder bin, inside the main installation folder, to the
   Windows PATH environment variable. This may be done automatically by
   the installer

-  

   -  To make sure everything went well run the command *doxygen --help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

Md5sum

-  Needed for consistency cheks

-  `Download and install from the official
   web-page <http://www.etree.org/md5com.html>`__

-  Add the folder where you decompress the executable to the Windows
   PATH environment variable.

-  

   -  To make sure everything went well run the command *md5sum --help*
      in a Windows Command Prompt. If the command is recognized you are
      good to go.

Windows cmake toolchain
-----------------------

| Similarly to Linux, in order to build a QUASAR server in Windows, you
  need to provide a toolchain file, so cmake knows where to find all the
  different dependancies. Since this dependancies are not as common as
  in Linux, this makes the toolchain even more complex.
| Taking this into account, a default toolchain for windows is provided,
  so the user can easily build in a Windows system. `This file is
  available in the Documentation folder of quasar, with the name
  enice_win_configuration. <enice_win_configuration.cmake>`__

Default windows toolchain
-------------------------

The provided default windows toolchain will take the paths to the
dependancies from the following places:

-  The Boost installation path needs to be specified trough an
   environment variable called BOOST_HOME.

-  

   -  If the environment variable BOOST_HOME is not available an error
      will be shown and the compiling will fail

-  The Unified Automation toolkit installation path needs to be
   specified trough an environment variable called
   UNIFIED_AUTOMATION_HOME.

-  

   -  If the environment variable UNIFIED_AUTOMATION_HOME is not
      available an error will be shown and the compiling will fail

-  The Code Synthesis XSD installation path needs to be specified trough
   an environment variable called CODE_SYNTHESYS_XSD.

-  

   -  If the environment variable CODE_SYNTHESYS_XSD is not available a
      warning will be shown and the default path of C:/Program Files
      (x86)/CodeSynthesis XSD 3.3 will be used

-  The OpenSSL installation path needs to be specified trough an
   environment variable called OPENSSL.

-  

   -  If the environment variable OPENSSL is not available a warning
      will be shown and the default path of C:/OpenSSL-Win64 will be
      used

-  The Xerces-c installation path needs to be specified trough an
   environment variable called XERCESC.

-  

   -  If the environment variable XERCESC is not available a warning
      will be shown and the default path of
      C:/3rdPartySoftware/xerces-c-3.1.2/Build/Win64 will be used

-  The Lib XML 2 installation path needs to be specified trough an
   environment variable called LIBXML2.

-  

   -  If the environment variable LIBXML2 is not available a warning
      will be shown and the default path of
      C:/3rdPartySoftware/libxml2/v2.7.8_64BIT/BUILD will be used

Take into account, that if the folder structure of the dependancies is
different in your computer, the toolchain will fail, so the user should
modify it accordingly to his needs.
