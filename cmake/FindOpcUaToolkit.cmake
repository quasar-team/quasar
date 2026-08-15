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
# 1. UASDKCPP package config (UA SDK >= 2.0): lib/cmake/UASDKCPP under
#    OPCUA_TOOLKIT_PATH (variable or environment), or under the standard
#    prefixes when no path is given. Skipped entirely when the build config
#    pre-sets OPCUA_TOOLKIT_LIBS_RELEASE/_DEBUG.
# 2. Explicit path set in the build config: set(OPCUA_TOOLKIT_PATH "/path/to/toolkit")
# 3. Environment variable: export OPCUA_TOOLKIT_PATH=/path/to/toolkit
# 4. Standard system paths (/opt/OpcUaToolkit, /usr/local/OpcUaToolkit, etc.)
# 5. Paths from CMAKE_PREFIX_PATH
#
# Example usage in build config file:
#   # Option 1: Let the module find the toolkit automatically
#   find_package(OpcUaToolkit REQUIRED)
#
#   # Option 2: Explicitly set the path (preferred method)
#   set(OPCUA_TOOLKIT_PATH "/path/to/OpcUaToolkit")
#   find_package(OpcUaToolkit REQUIRED)

if("Client" IN_LIST OpcUaToolkit_FIND_COMPONENTS)

  function(_quasar_client_prod_version root out_var)
    set(_version "unknown")
    foreach(_flavour uabasecpp uabase)
      if(EXISTS "${root}/include/${_flavour}/uabase.h")
        file(STRINGS "${root}/include/${_flavour}/uabase.h" _prod_lines
          REGEX "^#define PROD_(MAJOR|MINOR|PATCH)[ \t]+[0-9]+")
        set(_major "")
        set(_minor "")
        set(_patch "")
        foreach(_line IN LISTS _prod_lines)
          if(_line MATCHES "PROD_MAJOR[ \t]+([0-9]+)")
            set(_major "${CMAKE_MATCH_1}")
          elseif(_line MATCHES "PROD_MINOR[ \t]+([0-9]+)")
            set(_minor "${CMAKE_MATCH_1}")
          elseif(_line MATCHES "PROD_PATCH[ \t]+([0-9]+)")
            set(_patch "${CMAKE_MATCH_1}")
          endif()
        endforeach()
        if(NOT _major STREQUAL "" AND NOT _minor STREQUAL "" AND NOT _patch STREQUAL "")
          set(_version "${_major}.${_minor}.${_patch}")
        endif()
        break()
      endif()
    endforeach()
    set(${out_var} "${_version}" PARENT_SCOPE)
  endfunction()

  if(DEFINED OPCUA_TOOLKIT_PATH)
    set(_QUASAR_CLIENT_ROOT "${OPCUA_TOOLKIT_PATH}")
  elseif(DEFINED ENV{OPCUA_TOOLKIT_PATH})
    set(_QUASAR_CLIENT_ROOT "$ENV{OPCUA_TOOLKIT_PATH}")
  endif()

  if(DEFINED _QUASAR_CLIENT_ROOT)
    find_package(UASDKCPP CONFIG QUIET COMPONENTS Client
      PATHS "${_QUASAR_CLIENT_ROOT}" NO_DEFAULT_PATH)
  else()
    find_package(UASDKCPP CONFIG QUIET COMPONENTS Client
      PATHS /opt/uasdk /opt/OpcUaToolkit /opt/Unified-Automation /opt/opcua
            /usr/local/OpcUaToolkit /usr/local/Unified-Automation /usr/local/opcua /usr/opcua)
  endif()

  set(OpcUaToolkit_Client_FOUND FALSE)
  set(_QUASAR_CLIENT_FAILURE "")

  if(UASDKCPP_FOUND AND TARGET UASDKCPP::Client)
    get_filename_component(OPCUATOOLKIT_PATH "${UASDKCPP_DIR}/../../.." ABSOLUTE)
    set(OPCUATOOLKIT_PATH "${OPCUATOOLKIT_PATH}" CACHE PATH "Path to OPC UA Toolkit installation")
    set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")
    _quasar_client_prod_version("${OPCUATOOLKIT_PATH}" OPCUATOOLKIT_VERSION)
    if(OPCUATOOLKIT_VERSION STREQUAL "unknown" AND UASDKCPP_VERSION)
      set(OPCUATOOLKIT_VERSION "${UASDKCPP_VERSION}")
    endif()

    function(_quasar_uasdkcpp_client_include_dirs root out_var)
      set(_stack "${root}")
      set(_visited "")
      set(_dirs "")
      while(_stack)
        list(POP_FRONT _stack _t)
        if(_t IN_LIST _visited OR NOT TARGET "${_t}")
          continue()
        endif()
        list(APPEND _visited "${_t}")
        get_target_property(_inc "${_t}" INTERFACE_INCLUDE_DIRECTORIES)
        if(_inc)
          list(APPEND _dirs ${_inc})
        endif()
        get_target_property(_deps "${_t}" INTERFACE_LINK_LIBRARIES)
        if(_deps)
          foreach(_d IN LISTS _deps)
            if(_d MATCHES "^UASDKCPP::")
              list(APPEND _stack "${_d}")
            endif()
          endforeach()
        endif()
      endwhile()
      list(REMOVE_DUPLICATES _dirs)
      set(${out_var} "${_dirs}" PARENT_SCOPE)
    endfunction()

    _quasar_uasdkcpp_client_include_dirs(UASDKCPP::Client OPCUATOOLKIT_INCLUDE_DIRS)
    list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${OPCUATOOLKIT_PATH}/include")
    list(REMOVE_DUPLICATES OPCUATOOLKIT_INCLUDE_DIRS)

    set(OPCUATOOLKIT_CLIENT_LIBRARIES UASDKCPP::Client)
    if(NOT TARGET OpcUaToolkit::Client)
      add_library(OpcUaToolkit::Client INTERFACE IMPORTED)
    endif()
    set_target_properties(OpcUaToolkit::Client PROPERTIES
      INTERFACE_LINK_LIBRARIES "UASDKCPP::Client"
      INTERFACE_INCLUDE_DIRECTORIES "${OPCUATOOLKIT_INCLUDE_DIRS}")
    set(OpcUaToolkit_Client_FOUND TRUE)

    message(STATUS "OPC UA Toolkit client configuration (UASDKCPP package config at ${UASDKCPP_DIR}):")
    message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
    message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")

  elseif(DEFINED _QUASAR_CLIENT_ROOT AND EXISTS "${_QUASAR_CLIENT_ROOT}/include/open62541_compat.h")
    set(OPCUATOOLKIT_PATH "${_QUASAR_CLIENT_ROOT}" CACHE PATH "Path to OPC UA Toolkit installation")
    set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")
    set(OPCUATOOLKIT_VERSION "open62541-compat")
    set(OPCUATOOLKIT_INCLUDE_DIRS
      "${OPCUATOOLKIT_PATH}/include"
      "${OPCUATOOLKIT_PATH}/include/uaclient")
    foreach(_compat_extra_dir
        "${OPCUATOOLKIT_PATH}/extern/open62541/include"
        "${OPCUATOOLKIT_PATH}/build/open62541")
      if(EXISTS "${_compat_extra_dir}")
        list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${_compat_extra_dir}")
      endif()
    endforeach()

    find_library(_QUASAR_CLIENT_COMPAT_LIB open62541-compat
      PATHS "${OPCUATOOLKIT_PATH}/lib" "${OPCUATOOLKIT_PATH}/lib64" "${OPCUATOOLKIT_PATH}/build"
      NO_DEFAULT_PATH)
    find_library(_QUASAR_CLIENT_O6_LIB open62541
      PATHS "${OPCUATOOLKIT_PATH}/lib" "${OPCUATOOLKIT_PATH}/lib64" "${OPCUATOOLKIT_PATH}/build"
            "${OPCUATOOLKIT_PATH}/build/open62541"
      NO_DEFAULT_PATH)
    find_library(_QUASAR_CLIENT_LOGIT_LIB LogIt
      PATHS "${OPCUATOOLKIT_PATH}/lib" "${OPCUATOOLKIT_PATH}/lib64" "${OPCUATOOLKIT_PATH}/build"
            "${OPCUATOOLKIT_PATH}/build/LogIt"
      NO_DEFAULT_PATH)

    if(_QUASAR_CLIENT_COMPAT_LIB)
      set(OPCUATOOLKIT_CLIENT_LIBRARIES "${_QUASAR_CLIENT_COMPAT_LIB}")
      if(_QUASAR_CLIENT_O6_LIB)
        list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES "${_QUASAR_CLIENT_O6_LIB}")
      endif()
      if(_QUASAR_CLIENT_LOGIT_LIB)
        list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES "${_QUASAR_CLIENT_LOGIT_LIB}")
      endif()
      list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES -lpthread)

      if(NOT TARGET OpcUaToolkit::Client)
        add_library(OpcUaToolkit::Client INTERFACE IMPORTED)
      endif()
      set_target_properties(OpcUaToolkit::Client PROPERTIES
        INTERFACE_LINK_LIBRARIES "${OPCUATOOLKIT_CLIENT_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${OPCUATOOLKIT_INCLUDE_DIRS}")
      set(OpcUaToolkit_Client_FOUND TRUE)

      message(STATUS "OPC UA Toolkit client configuration (open62541-compat, experimental):")
      message(STATUS "  Backend: ${OPCUATOOLKIT_VERSION} at ${OPCUATOOLKIT_PATH}")
      message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
      message(STATUS "  Libraries: ${OPCUATOOLKIT_CLIENT_LIBRARIES}")
    else()
      set(_QUASAR_CLIENT_FAILURE
        "open62541-compat install at ${OPCUATOOLKIT_PATH} has no libopen62541-compat under lib, lib64 or build")
    endif()

  else()
    if(DEFINED _QUASAR_CLIENT_ROOT AND NOT IS_DIRECTORY "${_QUASAR_CLIENT_ROOT}")
      set(_QUASAR_CLIENT_FAILURE
        "OPCUA_TOOLKIT_PATH does not exist: ${_QUASAR_CLIENT_ROOT}")
      set(_QUASAR_CLIENT_ROOT "")
    endif()
    if(NOT DEFINED _QUASAR_CLIENT_ROOT)
      find_path(_QUASAR_CLIENT_ROOT
        NAMES include/uaclientcpp/uaclientsdk.h include/uaclient/uaclientsdk.h
        PATHS /opt/uasdk /opt/OpcUaToolkit /opt/Unified-Automation /opt/opcua
              /usr/local/OpcUaToolkit /usr/local/Unified-Automation /usr/local/opcua /usr/opcua)
    endif()

    if(_QUASAR_CLIENT_ROOT)
      set(OPCUATOOLKIT_PATH "${_QUASAR_CLIENT_ROOT}" CACHE PATH "Path to OPC UA Toolkit installation")
      set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")
      _quasar_client_prod_version("${OPCUATOOLKIT_PATH}" OPCUATOOLKIT_VERSION)

      set(OPCUATOOLKIT_INCLUDE_DIRS "")
      foreach(_flavours "uaclientcpp;uaclient" "uabasecpp;uabase" "uastack;uastack"
                        "uapkicpp;uapki" "xmlparsercpp;xmlparser")
        foreach(_flavour IN LISTS _flavours)
          if(EXISTS "${OPCUATOOLKIT_PATH}/include/${_flavour}")
            list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${OPCUATOOLKIT_PATH}/include/${_flavour}")
            break()
          endif()
        endforeach()
      endforeach()
      list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${OPCUATOOLKIT_PATH}/include")
      list(REMOVE_DUPLICATES OPCUATOOLKIT_INCLUDE_DIRS)

      if(EXISTS "${OPCUATOOLKIT_PATH}/lib")
        set(_QUASAR_CLIENT_LIB_DIR "${OPCUATOOLKIT_PATH}/lib")
      elseif(EXISTS "${OPCUATOOLKIT_PATH}/lib64")
        set(_QUASAR_CLIENT_LIB_DIR "${OPCUATOOLKIT_PATH}/lib64")
      else()
        set(_QUASAR_CLIENT_LIB_DIR "${OPCUATOOLKIT_PATH}/lib")
      endif()

      set(_QUASAR_CLIENT_STATIC_LIBS "")
      set(_QUASAR_CLIENT_SHARED_LIBS "")
      set(_QUASAR_CLIENT_MISSING "")
      foreach(_flavours "uaclientcpp;uaclient" "uabasecpp;uabase" "uastack"
                        "uapkicpp;uapki" "xmlparsercpp;xmlparser" "embeddedstack")
        set(_resolved "")
        foreach(_flavour IN LISTS _flavours)
          if(EXISTS "${_QUASAR_CLIENT_LIB_DIR}/lib${_flavour}.a")
            list(APPEND _QUASAR_CLIENT_STATIC_LIBS "${_QUASAR_CLIENT_LIB_DIR}/lib${_flavour}.a")
            set(_resolved "${_flavour}")
            break()
          elseif(EXISTS "${_QUASAR_CLIENT_LIB_DIR}/lib${_flavour}.so")
            list(APPEND _QUASAR_CLIENT_SHARED_LIBS "${_QUASAR_CLIENT_LIB_DIR}/lib${_flavour}.so")
            set(_resolved "${_flavour}")
            break()
          endif()
        endforeach()
        list(GET _flavours 0 _first_flavour)
        if(NOT _resolved AND NOT _first_flavour STREQUAL "uapkicpp"
           AND NOT _first_flavour STREQUAL "xmlparsercpp"
           AND NOT _first_flavour STREQUAL "embeddedstack")
          string(REPLACE ";" "|lib" _alternatives "${_flavours}")
          list(APPEND _QUASAR_CLIENT_MISSING "lib${_alternatives}")
        endif()
      endforeach()

      if(_QUASAR_CLIENT_MISSING)
        string(REPLACE ";" ", " _missing_pretty "${_QUASAR_CLIENT_MISSING}")
        set(_QUASAR_CLIENT_FAILURE
          "UA SDK client closure incomplete at ${_QUASAR_CLIENT_LIB_DIR} (parsed toolkit version ${OPCUATOOLKIT_VERSION}), missing artifacts: ${_missing_pretty}")
      elseif(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(_QUASAR_CLIENT_FAILURE
          "the Client component's legacy library search links with GNU ld group options and supports Linux only (host is ${CMAKE_SYSTEM_NAME}), use a UA SDK install with a UASDKCPP package config")
      else()
        set(OPCUATOOLKIT_CLIENT_LIBRARIES "")
        if(_QUASAR_CLIENT_STATIC_LIBS)
          list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES "-Wl,--start-group")
          list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES ${_QUASAR_CLIENT_STATIC_LIBS})
          list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES "-Wl,--end-group")
        endif()
        list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES ${_QUASAR_CLIENT_SHARED_LIBS})
        list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES -lxml2 -lssl -lcrypto -lpthread -lrt)
        find_library(_QUASAR_CLIENT_UUID_LIB uuid)
        if(_QUASAR_CLIENT_UUID_LIB)
          list(APPEND OPCUATOOLKIT_CLIENT_LIBRARIES "${_QUASAR_CLIENT_UUID_LIB}")
        endif()

        if(NOT TARGET OpcUaToolkit::Client)
          add_library(OpcUaToolkit::Client INTERFACE IMPORTED)
        endif()
        set_target_properties(OpcUaToolkit::Client PROPERTIES
          INTERFACE_LINK_LIBRARIES "${OPCUATOOLKIT_CLIENT_LIBRARIES}"
          INTERFACE_INCLUDE_DIRECTORIES "${OPCUATOOLKIT_INCLUDE_DIRS}")
        set(OpcUaToolkit_Client_FOUND TRUE)

        message(STATUS "OPC UA Toolkit client configuration (legacy library search):")
        message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
        message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
        message(STATUS "  Libraries: ${OPCUATOOLKIT_CLIENT_LIBRARIES}")
      endif()
    elseif(NOT _QUASAR_CLIENT_FAILURE)
      set(_QUASAR_CLIENT_FAILURE
        "no OPC UA toolkit found, set OPCUA_TOOLKIT_PATH to a UA SDK or open62541-compat install")
    endif()
  endif()

  if(_QUASAR_CLIENT_FAILURE)
    message(STATUS "OPC UA Toolkit client: ${_QUASAR_CLIENT_FAILURE}")
  endif()

  set(_QUASAR_CLIENT_FPHSA_REASON "")
  if(_QUASAR_CLIENT_FAILURE AND CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
    set(_QUASAR_CLIENT_FPHSA_REASON REASON_FAILURE_MESSAGE "${_QUASAR_CLIENT_FAILURE}")
  endif()
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(OpcUaToolkit
    REQUIRED_VARS OPCUATOOLKIT_PATH
    VERSION_VAR OPCUATOOLKIT_VERSION
    ${_QUASAR_CLIENT_FPHSA_REASON}
    HANDLE_COMPONENTS)
  return()
endif()

# --- UASDKCPP package-config discovery (UA SDK >= 2.0) -----------------------
# UA SDK 2.x installs export find_package(UASDKCPP) with imported targets
# (UASDKCPP::Server aggregates coremodule -> uabasecpp -> uastack/embeddedstack,
# including the transitive LibXml2/OpenSSL/pthread/rt link edges). Config mode
# is attempted first; 1.x installs (no lib/cmake/UASDKCPP) and build configs
# that pre-set the link lists fall through to the legacy search below, unchanged.
if(NOT DEFINED OPCUA_TOOLKIT_LIBS_RELEASE AND NOT DEFINED OPCUA_TOOLKIT_LIBS_DEBUG)
  if(DEFINED OPCUA_TOOLKIT_PATH)
    set(_QUASAR_UASDK_CONFIG_ROOT "${OPCUA_TOOLKIT_PATH}")
  elseif(DEFINED ENV{OPCUA_TOOLKIT_PATH})
    set(_QUASAR_UASDK_CONFIG_ROOT "$ENV{OPCUA_TOOLKIT_PATH}")
  endif()
  if(DEFINED _QUASAR_UASDK_CONFIG_ROOT)
    # NO_DEFAULT_PATH: the requested toolkit must not be shadowed by a stray
    # system-wide SDK picked up from CMake's default search.
    find_package(UASDKCPP CONFIG QUIET COMPONENTS Server
      PATHS "${_QUASAR_UASDK_CONFIG_ROOT}" NO_DEFAULT_PATH)
    if(NOT UASDKCPP_FOUND AND EXISTS "${_QUASAR_UASDK_CONFIG_ROOT}/lib/cmake/UASDKCPP")
      message(WARNING
        "A UASDKCPP package config exists at ${_QUASAR_UASDK_CONFIG_ROOT}/lib/cmake/UASDKCPP "
        "but find_package(UASDKCPP CONFIG COMPONENTS Server) did not succeed (the config "
        "failed to load or the Server component's targets are missing). Falling back to the "
        "legacy library search, which assumes a UA SDK 1.x layout and is likely to fail at "
        "link time against a 2.x install.")
    endif()
    unset(_QUASAR_UASDK_CONFIG_ROOT)
  else()
    find_package(UASDKCPP CONFIG QUIET COMPONENTS Server
      PATHS /opt/uasdk /opt/OpcUaToolkit /opt/Unified-Automation /opt/opcua
            /usr/local/OpcUaToolkit /usr/local/Unified-Automation /usr/local/opcua /usr/opcua)
  endif()
endif()

if(UASDKCPP_FOUND)
  # UASDKCPP_DIR is <prefix>/lib/cmake/UASDKCPP; derive the install prefix.
  get_filename_component(OPCUATOOLKIT_PATH "${UASDKCPP_DIR}/../../.." ABSOLUTE)
  set(OPCUATOOLKIT_PATH "${OPCUATOOLKIT_PATH}" CACHE PATH "Path to OPC UA Toolkit installation")
  set(OPCUA_TOOLKIT_PATH "${OPCUATOOLKIT_PATH}")
  set(OPCUATOOLKIT_VERSION "${UASDKCPP_VERSION}")

  # quasar's OBJECT modules compile against the toolkit headers via the global
  # include path, so hoist the imported targets' interface include dirs.
  function(_quasar_uasdk_collect_include_dirs root out_var)
    set(_stack "${root}")
    set(_visited "")
    set(_dirs "")
    while(_stack)
      list(POP_FRONT _stack _t)
      if(_t IN_LIST _visited OR NOT TARGET "${_t}")
        continue()
      endif()
      list(APPEND _visited "${_t}")
      get_target_property(_inc "${_t}" INTERFACE_INCLUDE_DIRECTORIES)
      if(_inc)
        list(APPEND _dirs ${_inc})
      endif()
      get_target_property(_deps "${_t}" INTERFACE_LINK_LIBRARIES)
      if(_deps)
        foreach(_d IN LISTS _deps)
          if(_d MATCHES "^UASDKCPP::")
            list(APPEND _stack "${_d}")
          endif()
        endforeach()
      endif()
    endwhile()
    list(REMOVE_DUPLICATES _dirs)
    set(${out_var} "${_dirs}" PARENT_SCOPE)
  endfunction()

  _quasar_uasdk_collect_include_dirs(UASDKCPP::Server OPCUATOOLKIT_INCLUDE_DIRS)
  list(APPEND OPCUATOOLKIT_INCLUDE_DIRS "${OPCUATOOLKIT_PATH}/include")
  list(REMOVE_DUPLICATES OPCUATOOLKIT_INCLUDE_DIRS)
  include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})

  set(OPCUA_TOOLKIT_LIBS_RELEASE UASDKCPP::Server)
  set(OPCUA_TOOLKIT_LIBS_DEBUG UASDKCPP::Server)
  set(OPCUATOOLKIT_LIBRARIES_RELEASE ${OPCUA_TOOLKIT_LIBS_RELEASE})
  set(OPCUATOOLKIT_LIBRARIES_DEBUG ${OPCUA_TOOLKIT_LIBS_DEBUG})
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_DEBUG})
  else()
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_RELEASE})
  endif()
  set(OPCUATOOLKIT_LIBRARY_DIRS "${OPCUATOOLKIT_PATH}/lib")
  set(OPCUATOOLKIT_FOUND TRUE)

  message(STATUS "OPC UA Toolkit configuration (UASDKCPP package config at ${UASDKCPP_DIR}):")
  message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
  message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
  message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(OpcUaToolkit
    REQUIRED_VARS OPCUATOOLKIT_PATH
    VERSION_VAR OPCUATOOLKIT_VERSION
  )
  if(NOT TARGET quasar_opcua_backend_is_ready)
    add_custom_target(quasar_opcua_backend_is_ready)
  endif()
  return()
endif()
# --- legacy hand-search (UA SDK 1.x, or build configs pre-setting the libs) ---

if(DEFINED OPCUA_TOOLKIT_PATH)
  set(OPCUATOOLKIT_PATH ${OPCUA_TOOLKIT_PATH} CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Using predefined OPCUA_TOOLKIT_PATH: ${OPCUATOOLKIT_PATH}")
elseif(DEFINED ENV{OPCUA_TOOLKIT_PATH})
  set(OPCUATOOLKIT_PATH $ENV{OPCUA_TOOLKIT_PATH} CACHE PATH "Path to OPC UA Toolkit installation")
  message(STATUS "Taking OPC UA Toolkit path from the environment: ${OPCUATOOLKIT_PATH}")
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
      lib/libuamodule.lib
      lib/libuastack.lib
    PATHS 
      ${_OPCUATOOLKIT_SEARCH_PATHS}
    DOC "Path to OPC UA Toolkit installation"
  )
endif()

if(OPCUATOOLKIT_PATH)
  set(OPCUATOOLKIT_INCLUDE_DIRS
    ${OPCUATOOLKIT_PATH}/include/uastack
    ${OPCUATOOLKIT_PATH}/include/uabasecpp
    ${OPCUATOOLKIT_PATH}/include/uaservercpp
    ${OPCUATOOLKIT_PATH}/include/uapkicpp
    ${OPCUATOOLKIT_PATH}/include/xmlparsercpp
  )

  # Initialize to TRUE, then set to FALSE if any directory is missing
  set(OPCUATOOLKIT_INCLUDE_DIR_FOUND TRUE)
  foreach(INCLUDE_DIR ${OPCUATOOLKIT_INCLUDE_DIRS})
    if(NOT EXISTS "${INCLUDE_DIR}")
      set(OPCUATOOLKIT_INCLUDE_DIR_FOUND FALSE)
      message(STATUS "Missing required include directory: ${INCLUDE_DIR}")
    endif()
  endforeach()
  
  if(NOT OPCUATOOLKIT_INCLUDE_DIR_FOUND)
    # Try alternate include layout
    set(OPCUATOOLKIT_INCLUDE_DIRS ${OPCUATOOLKIT_PATH}/include)
    if(EXISTS "${OPCUATOOLKIT_INCLUDE_DIRS}")
      set(OPCUATOOLKIT_INCLUDE_DIR_FOUND TRUE)
      message(STATUS "Found OPC UA Toolkit with alternate include layout: ${OPCUATOOLKIT_INCLUDE_DIRS}")
    endif()
  endif()

  if(EXISTS "${OPCUATOOLKIT_PATH}/VERSION")
    file(READ "${OPCUATOOLKIT_PATH}/VERSION" OPCUATOOLKIT_VERSION)
    string(STRIP "${OPCUATOOLKIT_VERSION}" OPCUATOOLKIT_VERSION)
  elseif(OPCUATOOLKIT_PATH MATCHES ".*OpcUaToolkit-static-([0-9]+\\.[0-9]+\\.[0-9]+).*")
    set(OPCUATOOLKIT_VERSION "${CMAKE_MATCH_1}")
  else()
    set(OPCUATOOLKIT_VERSION "unknown")
  endif()

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
  
  # Check that library files exist and build library lists
  set(OPCUATOOLKIT_LIBRARIES_DEBUG "")
  set(OPCUATOOLKIT_LIBRARIES_FOUND TRUE)
  
  foreach(LIB ${_OPCUATOOLKIT_LIB_NAMES_DEBUG})
    set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.so")
    if(NOT EXISTS "${LIB_FILE}")
      set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.a")
      if(NOT EXISTS "${LIB_FILE}")
        set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.lib")
        if(NOT EXISTS "${LIB_FILE}")
          set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/${LIB}.lib")
          if(NOT EXISTS "${LIB_FILE}")
            message(STATUS "Warning: Could not find debug library file for ${LIB}")
            set(OPCUATOOLKIT_LIBRARIES_FOUND FALSE)
          endif()
        endif()
      endif()
    endif()
    if(WIN32 AND EXISTS "${LIB_FILE}")
      list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "${LIB_FILE}")
    else()
      list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "-l${LIB}")
    endif()
  endforeach()
  if(WIN32)
    list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG ws2_32 rpcrt4)
  else()
    list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "-lxml2 -lssl -lcrypto -lpthread")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      list(APPEND OPCUATOOLKIT_LIBRARIES_DEBUG "-lrt")
    endif()
  endif()

  set(OPCUATOOLKIT_LIBRARIES_RELEASE "")
  foreach(LIB ${_OPCUATOOLKIT_LIB_NAMES_RELEASE})
    set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.so")
    if(NOT EXISTS "${LIB_FILE}")
      set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.a")
      if(NOT EXISTS "${LIB_FILE}")
        set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/lib${LIB}.lib")
        if(NOT EXISTS "${LIB_FILE}")
          set(LIB_FILE "${_OPCUATOOLKIT_LIB_PATH}/${LIB}.lib")
          if(NOT EXISTS "${LIB_FILE}")
            message(STATUS "Warning: Could not find release library file for ${LIB}")
            set(OPCUATOOLKIT_LIBRARIES_FOUND FALSE)
          endif()
        endif()
      endif()
    endif()
    if(WIN32 AND EXISTS "${LIB_FILE}")
      list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "${LIB_FILE}")
    else()
      list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "-l${LIB}")
    endif()
  endforeach()
  if(WIN32)
    list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE ws2_32 rpcrt4)
  else()
    list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "-lxml2 -lssl -lcrypto -lpthread")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      list(APPEND OPCUATOOLKIT_LIBRARIES_RELEASE "-lrt")
    endif()
  endif()
  
  if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_DEBUG})
  else()
    set(OPCUATOOLKIT_LIBRARIES ${OPCUATOOLKIT_LIBRARIES_RELEASE})
  endif()
  
  set(OPCUATOOLKIT_LIBRARY_DIRS ${_OPCUATOOLKIT_LIB_PATH})
  
  if(OPCUATOOLKIT_INCLUDE_DIR_FOUND AND OPCUATOOLKIT_LIBRARIES_FOUND)
    set(OPCUATOOLKIT_FOUND TRUE)
  endif()
  
  set(OPCUA_TOOLKIT_PATH ${OPCUATOOLKIT_PATH})
  if(DEFINED OPCUA_TOOLKIT_LIBS_DEBUG)
    message(STATUS "Using OPCUA_TOOLKIT_LIBS_DEBUG from the build config: ${OPCUA_TOOLKIT_LIBS_DEBUG}")
  else()
    set(OPCUA_TOOLKIT_LIBS_DEBUG ${OPCUATOOLKIT_LIBRARIES_DEBUG})
  endif()
  if(DEFINED OPCUA_TOOLKIT_LIBS_RELEASE)
    message(STATUS "Using OPCUA_TOOLKIT_LIBS_RELEASE from the build config: ${OPCUA_TOOLKIT_LIBS_RELEASE}")
  else()
    set(OPCUA_TOOLKIT_LIBS_RELEASE ${OPCUATOOLKIT_LIBRARIES_RELEASE})
  endif()
  
  include_directories(${OPCUATOOLKIT_INCLUDE_DIRS})
  
  message(STATUS "OPC UA Toolkit configuration:")
  message(STATUS "  Version: ${OPCUATOOLKIT_VERSION}")
  message(STATUS "  Include dirs: ${OPCUATOOLKIT_INCLUDE_DIRS}")
  message(STATUS "  Library path: ${OPCUATOOLKIT_LIBRARY_DIRS}")
endif()

if(OPCUATOOLKIT_PATH AND NOT OPCUATOOLKIT_LIBRARIES)
  if(WIN32)
    set(OPCUATOOLKIT_LIBRARIES "libuamodule.lib;libcoremodule.lib;libuabasecpp.lib;libuastack.lib;libuapkicpp.lib;libxmlparsercpp.lib;ws2_32;rpcrt4")
    set(OPCUATOOLKIT_LIBRARIES_DEBUG "libuamoduled.lib;libcoremoduled.lib;libuabasecppd.lib;libuastackd.lib;libuapkicppd.lib;libxmlparsercppd.lib;ws2_32;rpcrt4")
  else()
    set(OPCUATOOLKIT_LIBRARIES "-luamodule -lcoremodule -luabasecpp -luastack -luapkicpp -lxmlparsercpp -lxml2 -lssl -lcrypto -lpthread")
    set(OPCUATOOLKIT_LIBRARIES_DEBUG "-luamoduled -lcoremoduled -luabasecppd -luastackd -luapkicppd -lxmlparsercppd -lxml2 -lssl -lcrypto -lpthread")
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      set(OPCUATOOLKIT_LIBRARIES "${OPCUATOOLKIT_LIBRARIES} -lrt")
      set(OPCUATOOLKIT_LIBRARIES_DEBUG "${OPCUATOOLKIT_LIBRARIES_DEBUG} -lrt")
    endif()
  endif()
  set(OPCUATOOLKIT_LIBRARIES_RELEASE ${OPCUATOOLKIT_LIBRARIES})
  message(STATUS "Using default library list for OPC UA Toolkit")
endif()

if(OPCUATOOLKIT_PATH AND NOT OPCUATOOLKIT_INCLUDE_DIRS)
  set(OPCUATOOLKIT_INCLUDE_DIRS 
    ${OPCUATOOLKIT_PATH}/include
    ${OPCUATOOLKIT_PATH}/include/uastack
    ${OPCUATOOLKIT_PATH}/include/uabasecpp
  )
  message(STATUS "Using default include paths for OPC UA Toolkit")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OpcUaToolkit
  REQUIRED_VARS OPCUATOOLKIT_PATH
  VERSION_VAR OPCUATOOLKIT_VERSION
)

if(NOT TARGET quasar_opcua_backend_is_ready)
  add_custom_target(quasar_opcua_backend_is_ready)
endif()
