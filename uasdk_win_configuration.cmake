# LICENSE:
# Copyright (c) 2026, CERN
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
# @author Paris Moschovakos
# @date 16-Jun-2026
#
# UASDK (Unified Automation UA Toolkit) build configuration for Windows / MSVC.
# The Boost / XSD / OpenSSL / Xerces-C / LibXml2 sections mirror
# open6_win_configuration.cmake (same x64 deps, provisioned by the
# setup-quasar-windows composite action). Only the OPCUA section differs: it
# targets the UA Toolkit at $ENV{OPCUA_TOOLKIT_PATH} instead of the bundled
# open62541, and detects the installed SDK layout (1.x vs 2.0.x) the same way
# the Linux uasdk-eval.cmake does -- so one config serves 1.6.5, 1.8.9 and
# 2.0.3 (see the OPCUA section below for the per-major include/lib sets).
# (NOTE: Windows libs are unprefixed -- uamodule.lib, not libuamodule.lib).

message("using build configuration from uasdk_win_configuration.cmake")

#-------
#Boost
#-------
set( IGNORE_DEFAULT_BOOST_SETUP ON ) # quasar boost finder not used, all inline here
if(DEFINED ENV{BOOST_PATH_HEADERS})
	SET( BOOST_PATH_HEADERS $ENV{BOOST_PATH_HEADERS} )
elseif( DEFINED ENV{BOOST_HOME} )
	SET( BOOST_PATH_HEADERS $ENV{BOOST_HOME}/include )
endif()
if(DEFINED ENV{BOOST_PATH_LIBS})
	SET( BOOST_PATH_LIBS $ENV{BOOST_PATH_LIBS} )
elseif( DEFINED ENV{BOOST_HOME} )
	SET( BOOST_PATH_LIBS $ENV{BOOST_HOME}/lib )
endif()
if( NOT BOOST_PATH_HEADERS OR NOT BOOST_PATH_LIBS )
	message( FATAL_ERROR "unable to determine boost paths from BOOST_PATH_HEADERS [$ENV{BOOST_PATH_HEADERS}] BOOST_PATH_LIBS [$ENV{BOOST_PATH_LIBS}] BOOST_HOME [$ENV{BOOST_HOME}]")
endif()
message(STATUS "BOOST - include [${BOOST_PATH_HEADERS}] libs [${BOOST_PATH_LIBS}]")
# BoostSetup is bypassed on Windows, so put the Boost headers on the include path here.
include_directories(${BOOST_PATH_HEADERS})

# Boost libs discovered by glob (same helper as open6_win_configuration.cmake)
# so this file survives Boost/toolset bumps; -mt-x64- never matches -mt-gd-.
function( quasar_find_msvc_boost_library TARGET_NAME COMPONENT )
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
		file(GLOB _candidates "${BOOST_PATH_LIBS}/*boost_${COMPONENT}-*-mt-gd-x64-*.lib")
	else()
		file(GLOB _candidates "${BOOST_PATH_LIBS}/*boost_${COMPONENT}-*-mt-x64-*.lib")
	endif()
	if( NOT _candidates )
		# Header-only in this Boost (e.g. system, stub removed in 1.89) -- nothing to link.
		message( STATUS "boost ${COMPONENT}: no compiled lib under [${BOOST_PATH_LIBS}], treating as header-only" )
		return()
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
set( BOOST_LIBS )
foreach( _t libboostlogsetup libboostlog libboostsystem libboostfilesystem libboostthread libboostprogramoptions libboostchrono libboostdatetime )
	if( TARGET ${_t} )
		list(APPEND BOOST_LIBS ${_t})
	endif()
endforeach()
message( STATUS "BOOST_LIBS resolved to [${BOOST_LIBS}]" )

#-----
# LogIt
#-----
SET( LOGIT_HAS_STDOUTLOG TRUE )
SET( LOGIT_HAS_BOOSTLOG FALSE )
SET( LOGIT_HAS_UATRACE FALSE )

#-----
#CodeSynthesis XSD
#-----
if( NOT DEFINED ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS} )
	message( FATAL_ERROR "unable to determine Code Synthesis headers from CODE_SYNTHESYS_XSD_PATH_HEADERS [$ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS}]")
endif()
include_directories($ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS})

#----
#OPENSSL / XERCESC / LIBXML2 (w2025 static builds, same as the o6 Windows config)
#----
if( NOT DEFINED ENV{OPENSSL_PATH_HEADERS} OR NOT DEFINED ENV{OPENSSL_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine openssl paths OPENSSL_PATH_HEADERS [$ENV{OPENSSL_PATH_HEADERS}] OPENSSL_PATH_LIBS [$ENV{OPENSSL_PATH_LIBS}]")
endif()
include_directories($ENV{OPENSSL_PATH_HEADERS})
if( NOT DEFINED ENV{XERCESC_PATH_HEADERS} OR NOT DEFINED ENV{XERCESC_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine xerces-c paths XERCESC_PATH_HEADERS [$ENV{XERCESC_PATH_HEADERS}] XERCESC_PATH_LIBS [$ENV{XERCESC_PATH_LIBS}]")
endif()
include_directories($ENV{XERCESC_PATH_HEADERS})

if(NOT TARGET libssl)
	add_library(libssl STATIC IMPORTED)
	set_property(TARGET libssl PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libssl.lib)
endif()
if(NOT TARGET libcrypto)
	add_library(libcrypto STATIC IMPORTED)
	set_property(TARGET libcrypto PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libcrypto.lib)
endif()
SET( OPENSSL_LIBS_ALL libssl libcrypto )

if(NOT TARGET libxercesc)
	add_library(libxercesc STATIC IMPORTED)
	set_property(TARGET libxercesc PROPERTY IMPORTED_LOCATION $ENV{XERCESC_PATH_LIBS}/xerces-c_3.lib)
endif()
if(NOT TARGET libxml2)
	add_library(libxml2 STATIC IMPORTED)
	set_property(TARGET libxml2 PROPERTY IMPORTED_LOCATION $ENV{LIBXML2_PATH_LIBS}/libxml2s.lib)
endif()
SET( XML_LIBS Rpcrt4 crypt32 ws2_32 libxercesc ${OPENSSL_LIBS_ALL} )

#------
#OPCUA -- Unified Automation UA Toolkit (UASDK)
#------
add_definitions( -DBACKEND_UATOOLKIT )
if( NOT DEFINED ENV{OPCUA_TOOLKIT_PATH} )
	message( FATAL_ERROR "OPCUA_TOOLKIT_PATH must point at the built UA SDK (the windows-uasdk job sets it after building the SDK from source)" )
endif()
SET( OPCUA_TOOLKIT_PATH $ENV{OPCUA_TOOLKIT_PATH} )
message(STATUS "Using UA Toolkit at OPCUA_TOOLKIT_PATH [${OPCUA_TOOLKIT_PATH}]")
# The installed UA SDK module set differs across majors -- same detection idea as the
# Linux uasdk-eval.cmake, keyed on the 1.x-only uamodule lib (unprefixed on Windows).
# 1.x (1.6.5/1.8.9) ships uamodule + separate uapkicpp/xmlparsercpp; 2.0.x folds
# pki + xmlparser into uabasecpp, folds uamodule into coremodule, and REQUIRES the
# additional low-level embeddedstack lib (ua_malloc/ua_buffer_*/ua_decoder_context_*,
# which uabasecpp links against). MSVC's linker resolves the mutually-referential
# 2.0.x static modules without ld-style --start-group, so a plain list suffices.
# Windows UASDK static libs are UNPREFIXED (uamodule.lib, not libuamodule.lib), with
# a 'd' debug suffix in the 1.x layout; the CI SDK artifact is a Release build, so
# the 2.0.x debug list mirrors release (as the Linux config does).
if( EXISTS "${OPCUA_TOOLKIT_PATH}/lib/uamodule.lib" )
	# UA SDK 1.x layout
	SET( _UASDK_INCDIRS uabasecpp uaservercpp uaclientcpp uamodels uapkicpp uastack xmlparsercpp )
	SET( OPCUA_TOOLKIT_LIBS_RELEASE uamodule.lib coremodule.lib uabasecpp.lib uastack.lib uapkicpp.lib xmlparsercpp.lib ws2_32 rpcrt4 crypt32 )
	SET( OPCUA_TOOLKIT_LIBS_DEBUG   uamoduled.lib coremoduled.lib uabasecppd.lib uastackd.lib uapkicppd.lib xmlparsercppd.lib ws2_32 rpcrt4 crypt32 )
else()
	# UA SDK 2.0.x layout
	SET( _UASDK_INCDIRS uabasecpp uaservercpp uastack uaclientcpp )
	SET( OPCUA_TOOLKIT_LIBS_RELEASE coremodule.lib uabasecpp.lib uastack.lib embeddedstack.lib ws2_32 rpcrt4 crypt32 )
	SET( OPCUA_TOOLKIT_LIBS_DEBUG   ${OPCUA_TOOLKIT_LIBS_RELEASE} )
endif()
include_directories( ${OPCUA_TOOLKIT_PATH}/include )
foreach( _d ${_UASDK_INCDIRS} )
	include_directories( ${OPCUA_TOOLKIT_PATH}/include/${_d} )
endforeach()
link_directories( ${OPCUA_TOOLKIT_PATH}/lib )

#------
#General
#------
# XERCES_STATIC_LIBRARY: the w2025 xerces-c is BUILD_SHARED_LIBS=OFF; without
# the define the xercesc headers declare dllimport symbols and the link fails.
add_definitions(-DSUPPORT_XML_CONFIG -DWIN32_LEAN_AND_MEAN -DXERCES_STATIC_LIBRARY)
# NOTE on the Windows msxml/Xerces DOMDocument clash: the UASDK platform headers pull in the
# full Windows API, which transitively includes <urlmon.h> + <msxml.h>; msxml's global
# ::DOMDocument coclass then collides with xercesc::DOMDocument inside CodeSynthesis libxsd's
# 'using namespace xercesc' blocks. We CANNOT skip msxml.h (urlmon.h uses IXMLElement from it),
# so the fix is applied at the libxsd level (the windows-uasdk job qualifies bare DOMDocument ->
# xercesc::DOMDocument before the quasar build). o6 never drags the COM chain into XSD TUs.
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin/)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin/)
