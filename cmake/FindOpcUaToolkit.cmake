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

if(DEFINED ENV{UNIFIED_AUTOMATION_HOME} AND NOT "$ENV{UNIFIED_AUTOMATION_HOME}" STREQUAL "")
  list(PREPEND CMAKE_PREFIX_PATH "$ENV{UNIFIED_AUTOMATION_HOME}")
endif()

find_package(UASDKCPP CONFIG REQUIRED
  COMPONENTS ${_OPCUATOOLKIT_REQUIRED_COMPONENTS}
)

get_filename_component(_OPCUATOOLKIT_PREFIX "${UASDKCPP_DIR}/../../.." ABSOLUTE)

set(OPCUATOOLKIT_INCLUDE_DIRS)
foreach(_target IN LISTS _OPCUATOOLKIT_COMPONENT_TARGETS)
  get_target_property(_includes "${_target}" INTERFACE_INCLUDE_DIRECTORIES)
  if(_includes)
    list(APPEND OPCUATOOLKIT_INCLUDE_DIRS ${_includes})
  endif()
endforeach()
list(REMOVE_DUPLICATES OPCUATOOLKIT_INCLUDE_DIRS)

set(OPCUA_TOOLKIT_LIBS_RELEASE UASDKCPP::Server)
set(OPCUA_TOOLKIT_LIBS_DEBUG UASDKCPP::Server)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_DEBUG})
else()
  set(OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_RELEASE})
endif()

set(OPCUATOOLKIT_VERSION "${UASDKCPP_VERSION}")
if(NOT OPCUATOOLKIT_VERSION)
  set(OPCUATOOLKIT_VERSION "unknown")
endif()
set(OPCUATOOLKIT_LIBRARY_DIRS "${_OPCUATOOLKIT_PREFIX}/lib")

set(OPCUATOOLKIT_FOUND TRUE)
set(OpcUaToolkit_FOUND TRUE)
set(OPCUA_TOOLKIT_PATH "${_OPCUATOOLKIT_PREFIX}")

include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})

message(STATUS "OPC UA Toolkit configuration:")
message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")

if(NOT TARGET quasar_opcua_backend_is_ready)
  add_custom_target(quasar_opcua_backend_is_ready)
endif()
