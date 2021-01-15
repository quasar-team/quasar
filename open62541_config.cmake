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
# Author: Ben Farnham
# @author pnikiel
# @date May-2018
# This file is a build config for quasar-based projects to use open62541 OPC-UA backend.
# You can use it directly, or make a copy and adjust accordingly.

#-----
#General settings
#-----

add_definitions(-Wall -Wno-deprecated)

# open62541-compat has no uatrace
set (LOGIT_HAS_UATRACE FALSE)

# open62541-compat with server config loader - load ServerConfig.xml by default
set (SERVERCONFIG_LOADER ON CACHE BOOL "Since quasar 1.5.1 the open62541-compat will also load ServerConfig.xml same way UA-SDK does")

# need C++11
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

#-------
#Boost
#-------
# should get resolved via FrameworkInternals/BoostSetup.cmake since quasar 1.3.13

#------
#OPCUA
#------

# No OPC-UA Toolkit: using Open62541-compat instead. It is referenced in BACKEND_MODULES below
add_definitions( -DBACKEND_OPEN62541 )
SET( OPCUA_TOOLKIT_PATH "" )
SET( OPCUA_TOOLKIT_LIBS_RELEASE -lrt -lpthread )
SET( OPCUA_TOOLKIT_LIBS_DEBUG -lrt -lpthread )
include_directories( ${PROJECT_BINARY_DIR}/open62541-compat/extern/open62541/include )

#-----
#XML Libs
#-----
#As of 03-Sep-2015 I see no FindXerces or whatever in our Cmake 2.8 installation, so no find_package can be user...
# TODO perhaps also take it from environment if requested
SET( XML_LIBS "-lxerces-c" )

#-----
#Quasar server libs
#-----
SET( QUASAR_SERVER_LIBS "-lssl -lcrypto -lpthread" )
