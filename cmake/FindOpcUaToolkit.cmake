# LICENSE:
# Copyright (c) 2025, CERN
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
# FindOpcUaToolkit.cmake
#
# Created on: 29 April 2025
# Author: Paris Moschovakos <paris.moschovakos@cern.ch>
#
# CMake module to find and configure the OPC UA Toolkit.
# This module will find the OPC UA Toolkit in the system and set up all the
# needed variables for compilation, linking, and include directories.
#
# Search order:
# 1. Explicit path set in the build config: set(OPCUA_TOOLKIT_PATH "/path/to/toolkit")
# 2. Environment variable: export OPCUA_TOOLKIT_PATH=/path/to/toolkit
# 3. Standard system paths (/opt/OpcUaToolkit, /usr/local/OpcUaToolkit, etc.)
# 4. Paths from CMAKE_PREFIX_PATH
#
# Example usage in build config file:
#   # Option 1: Let the module find the toolkit automatically
#   find_package(OpcUaToolkit REQUIRED)
#
#   # Option 2: Explicitly set the path (preferred method)
#   set(OPCUA_TOOLKIT_PATH "/path/to/OpcUaToolkit")
#   find_package(OpcUaToolkit REQUIRED)

if(DEFINED OPCUA_TOOLKIT_PATH)
  set(OPCUATOOLKIT_PATH ${OPCUA_TOOLKIT_PATH} CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Using predefined OPCUA_TOOLKIT_PATH: ${OPCUATOOLKIT_PATH}")
elseif(DEFINED ENV{OPCUA_TOOLKIT_PATH})
  set(OPCUATOOLKIT_PATH $ENV{OPCUA_TOOLKIT_PATH} CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Taking OPC UA Toolkit path from the environment: ${OPCUATOOLKIT_PATH}")
elseif(DEFINED ENV{UNIFIED_AUTOMATION_HOME})
  set(OPCUATOOLKIT_PATH $ENV{UNIFIED_AUTOMATION_HOME} CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Taking OPC UA Toolkit path from UNIFIED_AUTOMATION_HOME: ${OPCUATOOLKIT_PATH}")
else()
  set(_OPCUATOOLKIT_SEARCH_PATHS
    "/opt/OpcUaToolkit"
    "/opt/Unified-Automation"
    "/opt/opcua"
    "/usr/local/OpcUaToolkit"
    "/usr/local/Unified-Automation"
    "/usr/local/opcua"
    "/usr/opcua"
  )

  if(DEFINED ENV{UNIFIED_AUTOMATION_HOME})
    list(APPEND _OPCUATOOLKIT_SEARCH_PATHS $ENV{UNIFIED_AUTOMATION_HOME})
  endif()
  
  if(CMAKE_PREFIX_PATH)
    list(APPEND _OPCUATOOLKIT_SEARCH_PATHS ${CMAKE_PREFIX_PATH})
  endif()
  
  find_path(OPCUATOOLKIT_PATH
    NAMES 
      include/uabasecpp
      include/uastack
      include/uabasecpp/uadatavalue.h
      include/uastack/opcua_types.h
      include/uamodule/uamodule.h
      lib/libuamodule.so
      lib/libuastack.so
    PATHS 
      ${_OPCUATOOLKIT_SEARCH_PATHS}
    DOC "Path to OPC UA Toolkit installation"
  )
endif()

if(OPCUATOOLKIT_PATH)
  set(OPCUATOOLKIT_INCLUDE_DIRS
    ${OPCUATOOLKIT_PATH}/include
    ${OPCUATOOLKIT_PATH}/include/uastack
    ${OPCUATOOLKIT_PATH}/include/uabasecpp
    ${OPCUATOOLKIT_PATH}/include/uaservercpp
    ${OPCUATOOLKIT_PATH}/include/uabasedi
    ${OPCUATOOLKIT_PATH}/include/uaserverdi
  )
  set(_OPCUATOOLKIT_INCLUDE_DIRS_EXISTING)
  foreach(INCLUDE_DIR ${OPCUATOOLKIT_INCLUDE_DIRS})
    if(EXISTS "${INCLUDE_DIR}")
      list(APPEND _OPCUATOOLKIT_INCLUDE_DIRS_EXISTING "${INCLUDE_DIR}")
    endif()
  endforeach()
  if(_OPCUATOOLKIT_INCLUDE_DIRS_EXISTING)
    set(OPCUATOOLKIT_INCLUDE_DIRS ${_OPCUATOOLKIT_INCLUDE_DIRS_EXISTING})
    set(OPCUATOOLKIT_INCLUDE_DIR_FOUND TRUE)
  else()
    set(OPCUATOOLKIT_INCLUDE_DIR_FOUND FALSE)
  endif()

  if(EXISTS "${OPCUATOOLKIT_PATH}/VERSION")
    file(READ "${OPCUATOOLKIT_PATH}/VERSION" OPCUATOOLKIT_VERSION)
    string(STRIP "${OPCUATOOLKIT_VERSION}" OPCUATOOLKIT_VERSION)
  elseif(OPCUATOOLKIT_PATH MATCHES ".*OpcUaToolkit-static-([0-9]+\\.[0-9]+\\.[0-9]+).*")
    set(OPCUATOOLKIT_VERSION "${CMAKE_MATCH_1}")
  else()
    set(OPCUATOOLKIT_VERSION "unknown")
  endif()

  if(DEFINED ENV{LIBSSL_HOME})
    if(NOT DEFINED OPENSSL_ROOT_DIR)
      set(OPENSSL_ROOT_DIR "$ENV{LIBSSL_HOME}")
    endif()
    if(NOT DEFINED OPENSSL_USE_STATIC_LIBS)
      set(OPENSSL_USE_STATIC_LIBS TRUE)
    endif()
    list(APPEND CMAKE_PREFIX_PATH "$ENV{LIBSSL_HOME}")
  endif()
  if(DEFINED ENV{LIBXML2_HOME})
    list(APPEND CMAKE_PREFIX_PATH "$ENV{LIBXML2_HOME}")
  endif()

  find_package(UASDKCPP CONFIG QUIET
    PATHS
      "${OPCUATOOLKIT_PATH}/lib/cmake/UASDKCPP"
      "${OPCUATOOLKIT_PATH}"
    NO_DEFAULT_PATH
  )
  if(NOT UASDKCPP_FOUND)
    find_package(UASDKCPP CONFIG QUIET)
  endif()

  if(UASDKCPP_FOUND AND TARGET UASDKCPP::Server)
    if(DEFINED UASDKCPP_VERSION AND NOT UASDKCPP_VERSION STREQUAL "")
      set(OPCUATOOLKIT_VERSION "${UASDKCPP_VERSION}")
    endif()

    find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)
    find_package(LibXml2 REQUIRED)
    find_package(Threads REQUIRED)

    set(OPCUATOOLKIT_LIBRARIES_RELEASE
      UASDKCPP::Server
      OpenSSL::SSL
      OpenSSL::Crypto
      LibXml2::LibXml2
      Threads::Threads
      rt
    )
    set(OPCUATOOLKIT_LIBRARIES_DEBUG ${OPCUATOOLKIT_LIBRARIES_RELEASE})
    set(OPCUATOOLKIT_LIBRARIES_FOUND TRUE)
    set(OPCUATOOLKIT_LIBRARY_DIRS "${OPCUATOOLKIT_PATH}/lib")
    message(STATUS "Using UASDKCPP package configuration from ${OPCUATOOLKIT_PATH}/lib/cmake/UASDKCPP")
  else()
    # Legacy fallback (UA SDK 1.x style libraries)
    set(_OPCUATOOLKIT_LIB_NAMES_RELEASE
      uamodule
      coremodule
      uabasecpp
      uastack
      uapkicpp
      xmlparsercpp
    )

    set(_OPCUATOOLKIT_LIB_NAMES_DEBUG
      uamoduled
      coremoduled
      uabasecppd
      uastackd
      uapkicppd
      xmlparsercppd
    )

    set(_OPCUATOOLKIT_POSSIBLE_LIB_PATHS
      "${OPCUATOOLKIT_PATH}/lib"
      "${OPCUATOOLKIT_PATH}/lib64"
      "${OPCUATOOLKIT_PATH}/lib/uastack"
    )

    foreach(LIB_PATH ${_OPCUATOOLKIT_POSSIBLE_LIB_PATHS})
      if(EXISTS "${LIB_PATH}")
        set(_OPCUATOOLKIT_LIB_PATH "${LIB_PATH}")
        break()
      endif()
    endforeach()

    if(NOT _OPCUATOOLKIT_LIB_PATH)
      set(_OPCUATOOLKIT_LIB_PATH "${OPCUATOOLKIT_PATH}/lib")
    endif()

    set(OPCUATOOLKIT_LIBRARIES_DEBUG "")
    set(OPCUATOOLKIT_LIBRARIES_FOUND TRUE)

    foreach(LIB ${_OPCUATOOLKIT_LIB_NAMES_DEBUG})
      set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.so")
      if(NOT EXISTS "${LIB_FILE}")
        set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.a")
        if(NOT EXISTS "${LIB_FILE}")
          message(STATUS "Warning: Could not find debug library file for ${LIB}")
          set(OPCUATOOLKIT_LIBRARIES_FOUND FALSE)
        endif()
      endif()
      list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "-l${LIB}")
    endforeach()
    list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "-lxml2" "-lssl" "-lcrypto" "-lpthread" "-lrt")

    set(OPCUATOOLKIT_LIBRARIES_RELEASE "")
    foreach(LIB ${_OPCUATOOLKIT_LIB_NAMES_RELEASE})
      set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.so")
      if(NOT EXISTS "${LIB_FILE}")
        set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.a")
        if(NOT EXISTS "${LIB_FILE}")
          message(STATUS "Warning: Could not find release library file for ${LIB}")
          set(OPCUATOOLKIT_LIBRARIES_FOUND FALSE)
        endif()
      endif()
      list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "-l${LIB}")
    endforeach()
    list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "-lxml2" "-lssl" "-lcrypto" "-lpthread" "-lrt")

    set(OPCUATOOLKIT_LIBRARY_DIRS ${_OPCUATOOLKIT_LIB_PATH})
    message(STATUS "Using legacy OPC UA Toolkit library discovery")
  endif()

  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_DEBUG})
  else()
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_RELEASE})
  endif()

  if(OPCUATOOLKIT_INCLUDE_DIR_FOUND AND OPCUATOOLKIT_LIBRARIES_FOUND)
    set(OPCUATOOLKIT_FOUND TRUE)
  endif()

  set(OPCUA_TOOLKIT_PATH ${OPCUATOOLKIT_PATH})
  set(OPCUA_TOOLKIT_LIBS_DEBUG ${OPCUATOOLKIT_LIBRARIES_DEBUG})
  set(OPCUA_TOOLKIT_LIBS_RELEASE ${OPCUATOOLKIT_LIBRARIES_RELEASE})

  include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})

  message(STATUS "OPC UA Toolkit configuration:")
  message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
  message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
  message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpcUaToolkit
  REQUIRED_VARS OPCUATOOLKIT_PATH
  VERSION_VAR OPCUATOOLKIT_VERSION
)

if(NOT TARGET quasar_opcua_backend_is_ready)
  add_custom_target(quasar_opcua_backend_is_ready)
endif()
