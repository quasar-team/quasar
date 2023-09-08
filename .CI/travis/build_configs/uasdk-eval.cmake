# Please see http://quasar.docs.cern.ch/quasarBuildSystem.html for
# information how to use this file.
# LICENSE:
# Copyright (c) 2015, CERN
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Author: Piotr Nikiel <piotr@nikiel.info>
# @author pnikiel
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.

#-------
#Boost
#-------
# this will be resolved by BoostSetup.cmake since quasar 1.3.13, nothing to do

#------
#OPCUA
#------

SET( OPCUA_TOOLKIT_PATH /opt/uasdk )
SET( OPCUA_TOOLKIT_LIBS_DEBUG "-luamoduled -lcoremoduled -luabasecppd -luastackd -luapkicppd -lxmlparsercppd -lxml2 -lssl -lcrypto -lpthread" )
SET( OPCUA_TOOLKIT_LIBS_RELEASE "-luamodule -lcoremodule -luabasecpp -luastack -luapkicpp -lxmlparsercpp -lxml2 -lssl -lcrypto -lpthread" )

add_custom_target( quasar_opcua_backend_is_ready )

include_directories (
        ${OPCUA_TOOLKIT_PATH}/include/platform/linux
        ${OPCUA_TOOLKIT_PATH}/include
        ${OPCUA_TOOLKIT_PATH}/include/uabasecpp
        ${OPCUA_TOOLKIT_PATH}/include/uaservercpp
        ${OPCUA_TOOLKIT_PATH}/include/uapkicpp
        ${OPCUA_TOOLKIT_PATH}/include/uastack
        ${OPCUA_TOOLKIT_PATH}/include/xmlparsercpp
)

#-----
#XML Libs
#-----
#As of 03-Sep-2015 I see no FindXerces or whatever in our Cmake 2.8 installation, so no find_package can be user...
# TODO perhaps also take it from environment if requested
SET( XML_LIBS "-lxerces-c" )

#-----
#General settings
#-----

# TODO: split between Win / MSVC, perhaps MSVC has different notation for these
add_definitions(-Wall -Wno-deprecated -std=gnu++0x -DBACKEND_UATOOLKIT )
