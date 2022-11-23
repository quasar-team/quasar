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

message("No build configuration was specified. Assuming defaults from default_configuration.cmake")

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
    SET( BOOST_LIBS "-lboost_program_options-mt -lboost_thread-mt" )
endif(DEFINED ENV{BOOST_PATH_HEADERS})


#------
#OPCUA
#------
if(DEFINED ENV{OPCUA_TOOLKIT_PATH})
    message ("Taking OPC UA Toolkit path from the environment: $ENV{OPCUA_TOOLKIT_PATH}")
    SET( OPCUA_TOOLKIT_PATH $ENV{OPCUA_TOOLKIT_PATH} )
else(DEFINED ENV{OPCUA_TOOLKIT_PATH})
    message ("Taking OPC UA Toolkit path /opt/OpcUaToolkit-1.3.3 (ATLAS default)")
    SET( OPCUA_TOOLKIT_PATH "/opt/OpcUaToolkit-1.3.3" )
endif(DEFINED ENV{OPCUA_TOOLKIT_PATH})
    
SET( OPCUA_TOOLKIT_LIBS_DEBUG "-luamoduled -lcoremoduled -luabased -luastackd -luapkid -lxmlparserd -lxml2 -lssl -lcrypto -lpthread" ) 
SET( OPCUA_TOOLKIT_LIBS_RELEASE "-luamodule -lcoremodule -luabase -luastack -luapki -lxmlparser -lxml2 -lssl -lcrypto -lpthread" ) 

add_custom_target( quasar_opcua_backend_is_ready ) 

include_directories (
	${OPCUA_TOOLKIT_PATH}/include/uastack
	${OPCUA_TOOLKIT_PATH}/include/uabase
	${OPCUA_TOOLKIT_PATH}/include/uaserver
	${OPCUA_TOOLKIT_PATH}/include/xmlparser
	${OPCUA_TOOLKIT_PATH}/include/uapki
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

#-------
# Github replacement
#-------
# If used, replaces https://github.com/ in modules which are fetched on demand,
# this can be used if private mirror git servers are used instead of github.
# Example, edit and uncomment if required.
#
#set(GIT_SERVER_REPLACEMENT "ssh://git@gitlab.cern.ch:7999/" )
