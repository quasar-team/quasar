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
# This file is a build config for quasar-based projects to use Unified Automation OPC-UA backend.
# You can use it directly, or make a copy and adjust accordingly.

#-----
#General settings
#-----

add_definitions(-Wall -Wno-deprecated)

# UA-SDK 2.x has no uatrace backend integration in LogIt
set (LOGIT_HAS_UATRACE FALSE)

# Server config loader: load INI/XML server config by default
set (SERVERCONFIG_LOADER ON CACHE BOOL "Load UA-SDK server configuration from bin/ServerConfig.ini or bin/ServerConfig.xml")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------
#Boost
#-------
# should get resolved via FrameworkInternals/BoostSetup.cmake since quasar 1.3.13

#------
#OPCUA
#------

# UA Toolkit backend
add_definitions( -DBACKEND_UATOOLKIT )
if(DEFINED ENV{UNIFIED_AUTOMATION_HOME})
   set(OPCUA_TOOLKIT_PATH "$ENV{UNIFIED_AUTOMATION_HOME}")
else()
   set(OPCUA_TOOLKIT_PATH "/opt/unified-automation")
endif()
SET( OPCUA_TOOLKIT_LIBS_RELEASE -lrt -lpthread )
SET( OPCUA_TOOLKIT_LIBS_DEBUG -lrt -lpthread )
include_directories(
   ${OPCUA_TOOLKIT_PATH}/include/uabasecpp
   ${OPCUA_TOOLKIT_PATH}/include/uastack
   ${OPCUA_TOOLKIT_PATH}/include/uaservercpp
   ${OPCUA_TOOLKIT_PATH}/include/uabasedi
   ${OPCUA_TOOLKIT_PATH}/include/uaserverdi
)

#-----
#XML Libs
#-----
if(DEFINED ENV{XERCES_C_HOME})
   list(APPEND CMAKE_PREFIX_PATH $ENV{XERCES_C_HOME})
endif()

find_package(XercesC REQUIRED)
message(STATUS "Found xerces-c: ${XercesC_INCLUDE_DIRS}")
include_directories(${XercesC_INCLUDE_DIRS})
set(XML_LIBS ${XercesC_LIBRARIES})

#-----
#Quasar server libs
#-----
set(QUASAR_SERVER_LIBS
   -lpthread
)
