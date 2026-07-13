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
# @author Damian Abalo
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.
message("using build configuration from enice_win_configuration.cmake")

#-------
#Boost
#-------
message(STATUS "starting the quest for boost, relevant environment variables: BOOST_PATH_HEADERS [$ENV{BOOST_PATH_HEADERS}] BOOST_PATH_LIBS [$ENV{BOOST_PATH_LIBS}] BOOST_HOME [$ENV{BOOST_HOME}]")
set( IGNORE_DEFAULT_BOOST_SETUP ON ) # i.e. quasar boost finder not used, all inline in this file.
# headers path
if(DEFINED ENV{BOOST_PATH_HEADERS})
	SET( BOOST_PATH_HEADERS $ENV{BOOST_PATH_HEADERS} )
	message(STATUS "using BOOST_PATH_HEADERS from environment BOOST_PATH_HEADERS [$BOOST_PATH_HEADERS]")
else()
	if( DEFINED ENV{BOOST_HOME} )
		SET( BOOST_PATH_HEADERS $ENV{BOOST_HOME}/include )
		message(STATUS "using BOOST_PATH_HEADERS from environment BOOST_HOME [$BOOST_HOME] -> BOOST_PATH_HEADERS [$BOOST_PATH_HEADERS]")
	endif()
endif()

# libs path
if(DEFINED ENV{BOOST_PATH_LIBS})
	SET( BOOST_PATH_LIBS $ENV{BOOST_PATH_LIBS} )
	message(STATUS "using BOOST_PATH_LIBS from environment BOOST_PATH_LIBS [$BOOST_PATH_LIBS]")
else()
	if( DEFINED ENV{BOOST_HOME} )
		SET( BOOST_PATH_LIBS $ENV{BOOST_HOME}/lib )
		message(STATUS "using BOOST_PATH_LIBS from environment BOOST_HOME [$BOOST_HOME] -> BOOST_PATH_LIBS [$BOOST_PATH_LIBS]")
	endif()
endif()

# final check
if( NOT BOOST_PATH_HEADERS OR NOT BOOST_PATH_LIBS )
	message( FATAL_ERROR "unable to determine boost headers and library paths from environment variables BOOST_PATH_HEADERS [$ENV{BOOST_PATH_HEADERS}] BOOST_PATH_LIBS [$ENV{BOOST_PATH_LIBS}] BOOST_HOME [$ENV{BOOST_HOME}]")
endif()

message(STATUS "BOOST - include [${BOOST_PATH_HEADERS}] libs [${BOOST_PATH_LIBS}]")

# The default BoostSetup is bypassed on Windows (IGNORE_DEFAULT_BOOST_SETUP), so
# the Boost headers must be put on the include path here (e.g. boost/xpressive,
# used by AddressSpace/ASNodeManager.cpp).
include_directories(${BOOST_PATH_HEADERS})



# Boost libs discovered by glob from the flat BOOST_PATH_LIBS dir so this file
# survives Boost/toolset bumps (names like libboost_system-vc145-mt-x64-1_91.lib;
# the -mt-x64- pattern never matches the -mt-gd- debug variants).
function( quasar_find_msvc_boost_library TARGET_NAME COMPONENT )
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
		file(GLOB _candidates "${BOOST_PATH_LIBS}/*boost_${COMPONENT}-*-mt-gd-x64-*.lib")
	else()
		file(GLOB _candidates "${BOOST_PATH_LIBS}/*boost_${COMPONENT}-*-mt-x64-*.lib")
	endif()
	if( NOT _candidates )
		message( FATAL_ERROR "no static boost ${COMPONENT} library found under BOOST_PATH_LIBS [${BOOST_PATH_LIBS}]" )
	endif()
	list(GET _candidates 0 _lib)
	message( STATUS "boost ${COMPONENT} -> ${_lib}" )
	if(NOT TARGET ${TARGET_NAME})
		add_library(${TARGET_NAME} STATIC IMPORTED)
		set_property(TARGET ${TARGET_NAME} PROPERTY IMPORTED_LOCATION ${_lib})
	endif()
endfunction()

quasar_find_msvc_boost_library( libboostprogramoptions program_options )
quasar_find_msvc_boost_library( libboostsystem system )
quasar_find_msvc_boost_library( libboostfilesystem filesystem )
quasar_find_msvc_boost_library( libboostchrono chrono )
quasar_find_msvc_boost_library( libboostdatetime date_time )
quasar_find_msvc_boost_library( libboostthread thread )
quasar_find_msvc_boost_library( libboostlog log )
quasar_find_msvc_boost_library( libboostlogsetup log_setup )

set( BOOST_LIBS  libboostlogsetup libboostlog libboostsystem libboostfilesystem libboostthread libboostprogramoptions libboostchrono libboostdatetime )

#-----
# LogIt
#-----
SET( LOGIT_HAS_STDOUTLOG TRUE )
SET( LOGIT_HAS_BOOSTLOG FALSE )
SET( LOGIT_HAS_UATRACE FALSE )
MESSAGE( STATUS "LogIt build options: stdout [${LOGIT_HAS_STDOUTLOG}] boost [${LOGIT_HAS_BOOSTLOG}] uaTrace [${LOGIT_HAS_UATRACE}]" )

#-----
#CodeSynthesys XSD
#-----
if( NOT DEFINED ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS} )
	message( FATAL_ERROR "unable to determine Code Synthesis headers from environment variables CODE_SYNTHESYS_XSD_PATH_HEADERS [$ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS}]")
else()
	message(STATUS "Using environment variable for CODE_SYNTHESYS_XSD_PATH_HEADERS [$ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS}")
endif()
include_directories($ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS})

#----
#OPENSSL
#----
if( NOT DEFINED ENV{OPENSSL_PATH_HEADERS} OR NOT DEFINED ENV{OPENSSL_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine openssl headers and library paths from environment variables OPENSSL_PATH_HEADERS [$ENV{OPENSSL_PATH_HEADERS}] OPENSSL_PATH_LIBS [$ENV{OPENSSL_PATH_LIBS}]")
else()
	message( STATUS "using openssl headers [$ENV{OPENSSL_PATH_HEADERS}] libs [$ENV{OPENSSL_PATH_LIBS}]")
endif()
include_directories($ENV{OPENSSL_PATH_HEADERS})

#----
#XERCESC
#----
if( NOT DEFINED ENV{XERCESC_PATH_HEADERS} OR NOT DEFINED ENV{XERCESC_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine xerces-c headers and library paths from environment variables XERCESC_PATH_HEADERS [$ENV{XERCESC_PATH_HEADERS}] XERCESC_PATH_LIBS [$ENV{XERCESC_PATH_LIBS}]")
else()
	message( STATUS "using xerces-c headers and library paths from environment variables XERCESC_PATH_HEADERS [$ENV{XERCESC_PATH_HEADERS}] XERCESC_PATH_LIBS [$ENV{XERCESC_PATH_LIBS}]")
endif()
include_directories($ENV{XERCESC_PATH_HEADERS})

#----
#OPENSSL
#----
if(NOT TARGET libssl)
	add_library(libssl STATIC IMPORTED)
	set_property(TARGET libssl PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libssl.lib)
endif()
if(NOT TARGET libcrypto)
	add_library(libcrypto STATIC IMPORTED)
	set_property(TARGET libcrypto PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libcrypto.lib)
endif()

SET( OPENSSL_LIBS_ALL libssl libcrypto )

#-----
#XML Libs
#-----
if(NOT TARGET libxercesc)
	add_library(libxercesc STATIC IMPORTED)
	set_property(TARGET libxercesc PROPERTY IMPORTED_LOCATION $ENV{XERCESC_PATH_LIBS}/xerces-c_3.lib)
endif()
if(NOT TARGET libxml2)
	add_library(libxml2 STATIC IMPORTED)
	set_property(TARGET libxml2 PROPERTY IMPORTED_LOCATION $ENV{LIBXML2_PATH_LIBS}/libxml2s.lib)
endif()

SET( XML_LIBS Rpcrt4 crypt32 ws2_32 libxercesc ${OPENSSL_LIBS_ALL} )

#-----
#GoogleTest
#-----
include_directories( ${PROJECT_SOURCE_DIR}/GoogleTest/gtest/src/gtest/include )

#------
#OPCUA
#------
add_definitions( -DBACKEND_OPEN62541 )
SET( OPCUA_TOOLKIT_PATH "" )
include_directories(${PROJECT_BINARY_DIR}/open62541-compat/extern/open62541/include )
SET( OPCUA_TOOLKIT_LIBS_RELEASE "" )
SET( OPCUA_TOOLKIT_LIBS_DEBUG   "" )

#------
#General
#------
# XERCES_STATIC_LIBRARY: the w2025 xerces-c is BUILD_SHARED_LIBS=OFF; without
# the define the xercesc headers declare dllimport symbols and the link fails.
add_definitions(-DSUPPORT_XML_CONFIG -DWIN32_LEAN_AND_MEAN -DXERCES_STATIC_LIBRARY)

set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin/)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin/)

# (removed gcc/clang '-v' linker flag -- invalid for the MSVC linker)
