# Author: Piotr Nikiel <piotr.nikiel@cern.ch>
# Author: Giordon Stark

LICENSE = "LGPL"
LIC_FILES_CHKSUM = "file://LICENSE.TXT;md5=e94f6920e0f51ea34f43be88dc810edc"

# No information for SRC_URI yet (only an external source tree was specified)
SRC_URI = "git:///home/pnikiel/gitProjects/poky-quasar/;rev=master"
S = "${WORKDIR}/git"

inherit cmake pythonnative

DEPENDS = "boost python-lxml-native xsd-native xerces-c python-enum34-native python-six-native " 


# install it correctly, manually
do_install() {
   # install configuration files to /etc/quasar/*
  install -d ${D}${sysconfdir}/quasar/
  install -m 0755 ${B}/Configuration/Configuration.xsd ${D}${sysconfdir}/quasar/
  install -m 0755 ${S}/bin/config.xml ${D}${sysconfdir}/quasar/
  # install binary to /usr/bin
  install -d ${D}${bindir}/
  install -m 0755 ${B}/bin/OpcUaServer ${D}${bindir}/
}
