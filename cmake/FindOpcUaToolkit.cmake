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
# CMake module to discover UA SDK 2.x from the exported UASDKCPP package config.

set(_OPCUATOOLKIT_REQUIRED_COMPONENTS
  Server
  uastack
  uabasecpp
  coremodule
  uabasedi
  uaserverdi
)

set(_OPCUATOOLKIT_COMPONENT_TARGETS
  UASDKCPP::uastack
  UASDKCPP::uabasecpp
  UASDKCPP::coremodule
  UASDKCPP::uabasedi
  UASDKCPP::uaserverdi
)

set(_OPCUATOOLKIT_EXPLICIT_PATH "")
if(DEFINED OPCUA_TOOLKIT_PATH AND NOT OPCUA_TOOLKIT_PATH STREQUAL "")
  set(_OPCUATOOLKIT_EXPLICIT_PATH "${OPCUA_TOOLKIT_PATH}")
endif()

if(NOT _OPCUATOOLKIT_EXPLICIT_PATH STREQUAL "")
  set(OPCUATOOLKIT_PATH "${_OPCUATOOLKIT_EXPLICIT_PATH}" CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Using predefined OPCUA_TOOLKIT_PATH: ${OPCUATOOLKIT_PATH}")
  find_package(UASDKCPP CONFIG REQUIRED
    COMPONENTS ${_OPCUATOOLKIT_REQUIRED_COMPONENTS}
    PATHS
      "${OPCUATOOLKIT_PATH}/lib/cmake/UASDKCPP"
      "${OPCUATOOLKIT_PATH}"
    NO_DEFAULT_PATH
  )
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
    list(INSERT _OPCUATOOLKIT_SEARCH_PATHS 0 "$ENV{UNIFIED_AUTOMATION_HOME}")
  endif()

  if(CMAKE_PREFIX_PATH)
    list(APPEND _OPCUATOOLKIT_SEARCH_PATHS ${CMAKE_PREFIX_PATH})
  endif()

  find_package(UASDKCPP CONFIG REQUIRED
    COMPONENTS ${_OPCUATOOLKIT_REQUIRED_COMPONENTS}
    PATHS ${_OPCUATOOLKIT_SEARCH_PATHS}
  )
endif()

if(NOT DEFINED OPCUATOOLKIT_PATH OR OPCUATOOLKIT_PATH STREQUAL "")
  get_filename_component(OPCUATOOLKIT_PATH "${UASDKCPP_DIR}/../../.." ABSOLUTE)
endif()

set(OPCUATOOLKIT_INCLUDE_DIRS)
foreach(_target IN LISTS _OPCUATOOLKIT_COMPONENT_TARGETS)
  get_target_property(_includes "${_target}" INTERFACE_INCLUDE_DIRECTORIES)
  if(_includes)
    list(APPEND OPCUATOOLKIT_INCLUDE_DIRS ${_includes})
  endif()
endforeach()
list(REMOVE_DUPLICATES OPCUATOOLKIT_INCLUDE_DIRS)

set(OPCUATOOLKIT_LIBRARIES_RELEASE UASDKCPP::Server)
set(OPCUATOOLKIT_LIBRARIES_DEBUG UASDKCPP::Server)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_DEBUG})
else()
  set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_RELEASE})
endif()

set(OPCUATOOLKIT_VERSION "${UASDKCPP_VERSION}")
if(NOT OPCUATOOLKIT_VERSION)
  set(OPCUATOOLKIT_VERSION "unknown")
endif()
set(OPCUATOOLKIT_LIBRARY_DIRS "${OPCUATOOLKIT_PATH}/lib")

set(OPCUATOOLKIT_FOUND TRUE)
set(OpcUaToolkit_FOUND TRUE)
set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")

include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})

message(STATUS "OPC UA Toolkit configuration:")
message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")

if(NOT TARGET quasar_opcua_backend_is_ready)
  add_custom_target(quasar_opcua_backend_is_ready)
endif()
