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

# need C++11
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

#-------
#Boost
#-------
#We would prefer to use find_package for Boost but unfortunately this fails for SLC6.
#So we try environment variables, and if that fails, try sth really standard.
if(DEFINED ENV{BOOST_PATH_HEADERS})
    message ("BOOST_PATH_HEADERS is in your environment, taking it into account")
    if((NOT DEFINED ENV{BOOST_PATH_LIBS}) OR (NOT DEFINED ENV{BOOST_LIBS}))
        message (FATAL_ERROR "You have BOOST_PATH_HEADERS in your environment, but you are missing one of this: BOOST_PATH_LIBS BOOST_LIBS    Please fix it or use custom build configuration file ")
    endif((NOT DEFINED ENV{BOOST_PATH_LIBS}) OR (NOT DEFINED ENV{BOOST_LIBS}))
    SET( BOOST_PATH_HEADERS $ENV{BOOST_PATH_HEADERS} )
    SET( BOOST_PATH_LIBS $ENV{BOOST_PATH_LIBS} )
    SET( BOOST_LIBS $ENV{BOOST_LIBS} )
else(DEFINED ENV{BOOST_PATH_HEADERS})   
    message ("Assuming standard Boost installation as no BOOST_PATH_HEADERS is defined in your environment")
    SET( BOOST_PATH_HEADERS "" )
    SET( BOOST_PATH_LIBS "" )
    if( NOT DEFINED ENV{BOOST_LIBS} )
      SET( BOOST_LIBS "-lboost_program_options-mt -lboost_thread-mt -lpthread" )
    else()
      SET( BOOST_LIBS $ENV{BOOST_LIBS} )
    endif()
endif(DEFINED ENV{BOOST_PATH_HEADERS})

message ("using BOOST_LIBS [${BOOST_LIBS}]")

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
