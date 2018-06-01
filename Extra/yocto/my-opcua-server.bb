# Recipe created by recipetool
# This is the basis of a recipe and may need further editing in order to be fully functional.
# (Feel free to remove these comments when editing.)
#
# WARNING: the following LICENSE and LIC_FILES_CHKSUM values are best guesses - it is
# your responsibility to verify that the values are complete and correct.
# Author: Piotr Nikiel <piotr.nikiel@cern.ch>

DEPENDS = "boost python python-lxml-native xsd xerces-c "

FILES_${PN} = "\
	    /opt/QuasarServer/OpcUaServer \
	    /opt/QuasarServer/config.xml \
 	    /opt/QuasarServer/Configuration.xsd\
	    "

LICENSE = "LGPL"
LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=e94f6920e0f51ea34f43be88dc810edc"

# No information for SRC_URI yet (only an external source tree was specified)
SRC_URI = "git:///home/pnikiel/gitProjects/poky-quasar/;rev=master"
S = "${WORKDIR}/git"

inherit cmake pythonnative

# Specify any options you want to pass to cmake using EXTRA_OECMAKE:
EXTRA_OECMAKE = "-DCMAKE_INSTALL_PREFIX=/opt/QuasarServer"

