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
# Created on: 29 April 2025
# Author: Paris Moschovakos <paris.moschovakos@cern.ch>
# Author: Luis Miguens Fernandez <luis.miguens@cern.ch>
#
# CMake module to find and configure the OPC UA Toolkit.
# This module will find the OPC UA Toolkit in the system and set up all the
# needed variables for compilation, linking, and include directories.
#
# Search order:
# 1. Explicit path set in the build config: set(OPCUA_TOOLKIT_PATH "/path/to/toolkit")
# 2. Environment variable: export OPCUA_TOOLKIT_PATH=/path/to/toolkit or UNIFIED_AUTOMATION_HOME=/path/to/toolkit
# 3. Standard system path ("/opt/uasdk")
# 4. Paths from CMAKE_PREFIX_PATH
#
# Example usage in build config file:
#   # Option 1: Let the module find the toolkit automatically
#   find_package(OpcUaToolkit REQUIRED)
#
#   # Option 2: Explicitly set the path (preferred method)
#   set(OPCUA_TOOLKIT_PATH "/path/to/OpcUaToolkit")
#   find_package(OpcUaToolkit REQUIRED)

set(_OPCUATOOLKIT_INCLUDE_SUBDIRS
  uastack
  uabasecpp
  uaservercpp
  uapkicpp
  xmlparsercpp
)

set(_OPCUATOOLKIT_RELEASE_LIB_NAMES
  uamodule
  coremodule
  uabasecpp
  uastack
  uapkicpp
  xmlparsercpp
)

set(_OPCUATOOLKIT_DEBUG_LIB_NAMES
  uamoduled
  coremoduled
  uabasecppd
  uastackd
  uapkicppd
  xmlparsercppd
)

set(_OPCUATOOLKIT_HINTS)
foreach(_hint
    "${OPCUA_TOOLKIT_PATH}"
    "$ENV{OPCUA_TOOLKIT_PATH}"
    "$ENV{UNIFIED_AUTOMATION_HOME}"
    "/opt/uasdk"
    ${CMAKE_PREFIX_PATH})
  if(_hint AND NOT _hint STREQUAL "")
    list(APPEND _OPCUATOOLKIT_HINTS "${_hint}")
  endif()
endforeach()
list(REMOVE_DUPLICATES _OPCUATOOLKIT_HINTS)

find_path(OPCUATOOLKIT_INCLUDE_ROOT
  NAMES
    uabasecpp/uabase.h
    uaservercpp/uamodule.h
    uastack/opcua_types.h
  PATHS ${_OPCUATOOLKIT_HINTS}
  PATH_SUFFIXES include
  NO_DEFAULT_PATH
)

find_path(OPCUATOOLKIT_LIBRARY_DIR
  NAMES
    libuamodule.a
    libcoremodule.a
    libuabasecpp.a
    libuastack.a
  PATHS ${_OPCUATOOLKIT_HINTS}
  PATH_SUFFIXES lib
  NO_DEFAULT_PATH
)

if(OPCUATOOLKIT_INCLUDE_ROOT)
  get_filename_component(OPCUATOOLKIT_PATH "${OPCUATOOLKIT_INCLUDE_ROOT}/.." ABSOLUTE)
elseif(OPCUATOOLKIT_LIBRARY_DIR)
  get_filename_component(OPCUATOOLKIT_PATH "${OPCUATOOLKIT_LIBRARY_DIR}/.." ABSOLUTE)
endif()

set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")
set(OPCUATOOLKIT_LIBRARY_DIRS "${OPCUATOOLKIT_LIBRARY_DIR}")
set(OPCUATOOLKIT_VERSION "unknown")

set(OPCUATOOLKIT_INCLUDE_DIRS)
foreach(_subdir IN LISTS _OPCUATOOLKIT_INCLUDE_SUBDIRS)
  set(_include_dir "${OPCUATOOLKIT_INCLUDE_ROOT}/${_subdir}")
  if(EXISTS "${_include_dir}")
    list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${_include_dir}")
  endif()
endforeach()

set(OPCUATOOLKIT_LIBRARIES_RELEASE)
set(OPCUATOOLKIT_LIBRARIES_DEBUG)
list(LENGTH _OPCUATOOLKIT_RELEASE_LIB_NAMES _OPCUATOOLKIT_NUM_LIBS)
math(EXPR _OPCUATOOLKIT_LAST_INDEX "${_OPCUATOOLKIT_NUM_LIBS} - 1")

foreach(_index RANGE ${_OPCUATOOLKIT_LAST_INDEX})
  list(GET _OPCUATOOLKIT_RELEASE_LIB_NAMES ${_index} _release_name)
  list(GET _OPCUATOOLKIT_DEBUG_LIB_NAMES ${_index} _debug_name)

  unset(_OPCUATOOLKIT_RELEASE_LIBRARY CACHE)
  unset(_OPCUATOOLKIT_RELEASE_LIBRARY)
  find_library(_OPCUATOOLKIT_RELEASE_LIBRARY
    NAMES ${_release_name}
    PATHS "${OPCUATOOLKIT_LIBRARY_DIR}"
    NO_DEFAULT_PATH
  )
  if(_OPCUATOOLKIT_RELEASE_LIBRARY)
    list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "${_OPCUATOOLKIT_RELEASE_LIBRARY}")
  endif()

  unset(_OPCUATOOLKIT_DEBUG_LIBRARY CACHE)
  unset(_OPCUATOOLKIT_DEBUG_LIBRARY)
  find_library(_OPCUATOOLKIT_DEBUG_LIBRARY
    NAMES ${_debug_name}
    PATHS "${OPCUATOOLKIT_LIBRARY_DIR}"
    NO_DEFAULT_PATH
  )
  if(_OPCUATOOLKIT_DEBUG_LIBRARY)
    list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "${_OPCUATOOLKIT_DEBUG_LIBRARY}")
  elseif(_OPCUATOOLKIT_RELEASE_LIBRARY)
    list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "${_OPCUATOOLKIT_RELEASE_LIBRARY}")
  endif()
endforeach()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(OPCUATOOLKIT_LIBRARIES "${OPCUATOOLKIT_LIBRARIES_DEBUG}")
else()
  set(OPCUATOOLKIT_LIBRARIES "${OPCUATOOLKIT_LIBRARIES_RELEASE}")
endif()

set(OPCUA_TOOLKIT_LIBS_RELEASE "${OPCUATOOLKIT_LIBRARIES_RELEASE}")
set(OPCUA_TOOLKIT_LIBS_DEBUG "${OPCUATOOLKIT_LIBRARIES_DEBUG}")
set(OPCUA_TOOLKIT_LIBS "${OPCUATOOLKIT_LIBRARIES}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpcUaToolkit
  REQUIRED_VARS
    OPCUATOOLKIT_PATH
    OPCUATOOLKIT_INCLUDE_DIRS
    OPCUATOOLKIT_LIBRARIES_RELEASE
)

if(OpcUaToolkit_FOUND)
  set(OPCUATOOLKIT_FOUND TRUE)
  include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})

  message(STATUS "OPC UA Toolkit configuration:")
  message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
  message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
  message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")
else()
  set(OPCUATOOLKIT_FOUND FALSE)
endif()

if(NOT TARGET quasar_opcua_backend_is_ready)
  add_custom_target(quasar_opcua_backend_is_ready)
endif()
