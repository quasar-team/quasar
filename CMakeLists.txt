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

# Author: Piotr Nikiel <piotr@nikiel.info>
# Author: Damian Abalo Miron <damian.abalo@cern.ch>
# Author: Stefan Schlenker
# Author: Paris Moschovakos <paris.moschovakos@cern.ch>

cmake_minimum_required(VERSION 3.30)
project(opc_ua)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_link_options($<$<CXX_COMPILER_ID:MSVC>:/VERBOSE:LIB>)

set(SERVERCONFIG_LOADER OFF) ## TODO: this is not working with ON, investigate and fix

if(EXISTS ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake)
	  include( ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake )
else()
     message(STATUS "Extra build configuration has not been chosen.")
endif()

include( ProjectSettings.cmake )


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if(NOT DEFINED LOGIT_BACKEND_STDOUTLOG)
    set(LOGIT_BACKEND_STDOUTLOG TRUE)
endif()

if(NOT DEFINED LOGIT_BACKEND_BOOSTLOG)
    set(LOGIT_BACKEND_BOOSTLOG TRUE)
endif()

if(WIN32 AND NOT DEFINED LOGIT_BACKEND_WINDOWS_DEBUGGER)
    set(LOGIT_BACKEND_WINDOWS_DEBUGGER TRUE)
endif()

set(QUASAR_GLOBAL_COMPILE_DEFINITIONS
    SUPPORT_XML_CONFIG)
set(QUASAR_GLOBAL_COMPILE_OPTIONS)
set(QUASAR_GLOBAL_INCLUDE_DIRECTORIES)

if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
message("Building out-of-source")
message("PROJECT_SOURCE_DIR= ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR= ${PROJECT_BINARY_DIR}")
endif()

if(DEFINED ENV{XERCES_C_HOME})
   list(APPEND CMAKE_PREFIX_PATH $ENV{XERCES_C_HOME})
endif()
find_package(XercesC REQUIRED)
message(STATUS "Found xerces-c: ${XercesC_INCLUDE_DIRS}")
include_directories(${XercesC_INCLUDE_DIRS})

if(DEFINED ENV{XSD_HOME})
    include_directories($ENV{XSD_HOME}/include)
endif()

if(WIN32)
    add_definitions(-DNOMINMAX -DWIN32_LEAN_AND_MEAN)
endif()

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

if(DEFINED ENV{LIBSSL_HOME})
  if(NOT DEFINED OPENSSL_ROOT_DIR OR OPENSSL_ROOT_DIR STREQUAL "")
    set(OPENSSL_ROOT_DIR "$ENV{LIBSSL_HOME}")
  endif()
  if(NOT DEFINED OPENSSL_USE_STATIC_LIBS)
    set(OPENSSL_USE_STATIC_LIBS TRUE)
  endif()
  list(APPEND CMAKE_PREFIX_PATH "$ENV{LIBSSL_HOME}")
endif()
find_package(OpenSSL REQUIRED)
message(STATUS "Found OpenSSL: ${OpenSSL}")

IF(DEFINED ENV{LIBXML2_HOME})
  message(STATUS "Using libxml2 from path $ENV{LIBXML2_HOME}")
  list(APPEND CMAKE_PREFIX_PATH $ENV{LIBXML2_HOME})
endif()
find_package(LibXml2 CONFIG REQUIRED)
message(STATUS "Found libxml2: ${LIBXML2_INCLUDE_DIRS}")
include_directories(${LIBXML2_INCLUDE_DIRS})

if(${BUILD_SERVER_SHARED_LIB})
	list(APPEND QUASAR_GLOBAL_COMPILE_DEFINITIONS BUILDING_SHARED_OBJECT)
endif(${BUILD_SERVER_SHARED_LIB})

if(EXISTS "${PROJECT_SOURCE_DIR}/FrameworkInternals/EnabledModules/open62541-compat.url")
  set(BACKEND_OPEN62541 ON)
  add_definitions( -DBACKEND_OPEN62541 )
  message(STATUS "Using open62541 backend (enabled module)")
  list(APPEND QUASAR_GLOBAL_INCLUDE_DIRECTORIES
  include_directories( ${PROJECT_BINARY_DIR}/open62541-compat/extern/open62541/include )
  )
else()
  set(BACKEND_UATOOLKIT ON)
  add_definitions( -DBACKEND_UATOOLKIT )
  if(NOT DEFINED LOGIT_BACKEND_UATRACE)
    set(LOGIT_BACKEND_UATRACE TRUE)
  endif()
  find_package(OpcUaToolkit REQUIRED)
  message(STATUS "Using UA Toolkit backend with FindOpcUaToolkit")
endif()

if( ( NOT DEFINED IGNORE_DEFAULT_BOOST_SETUP ) OR ( IGNORE_DEFAULT_BOOST_SETUP STREQUAL "OFF" ) )
    message( STATUS "Setting up boost location and libraries using FrameworkInternals/BoostSetup.cmake")
    include( FrameworkInternals/BoostSetup.cmake )
else()
    message( STATUS "Automatic boost setup was set to be ignored (IGNORE_DEFAULT_BOOST_SETUP). Boost libs and includes must be defined elsewhere (e.g. in the build config)." )
    list(APPEND QUASAR_GLOBAL_INCLUDE_DIRECTORIES ${BOOST_PATH_HEADERS})  # fallback for old build configs
endif()

set(PYTHON_COMMAND python)
set(DESIGN_FILE ${PROJECT_BINARY_DIR}/Design/DesignWithMeta.xml)

list(APPEND QUASAR_GLOBAL_INCLUDE_DIRECTORIES
	${PROJECT_BINARY_DIR}/Device/generated
	${PROJECT_SOURCE_DIR}/Configuration  # this is to cover Configurator.h
	${PROJECT_BINARY_DIR}/Configuration
	${OPCUATOOLKIT_INCLUDE_DIRS}
	${SERVER_INCLUDE_DIRECTORIES}
)


set(QUASAR_TRANSFORM_SENSITIVE_FILES
	${PROJECT_SOURCE_DIR}/FrameworkInternals/transformDesign.py
	${PROJECT_SOURCE_DIR}/FrameworkInternals/DesignInspector.py
	${PROJECT_SOURCE_DIR}/FrameworkInternals/Oracle.py )
set(OPTIONAL_SERVER_MODULES)
include( FrameworkInternals/OptionalModules.cmake )

set(NATIVE_SERVER_MODULES AddressSpace Configuration Common Server Device Meta LogIt CalculatedVariables)
set(SERVER_MODULES ${NATIVE_SERVER_MODULES} ${OPTIONAL_SERVER_MODULES} ${CUSTOM_SERVER_MODULES})
set(OPTIONAL_MODULES_INCLUDES)
set(MODULES_OBJECTS)

# add all module include directories
set(MODULE_INCLUDES)
foreach(module ${SERVER_MODULES})
    FILE(GLOB SUBMODULE_INCLUDES ${PROJECT_SOURCE_DIR}/${module}/*/include)
    set(MODULE_INCLUDES ${MODULE_INCLUDES} ${PROJECT_SOURCE_DIR}/${module}/include ${PROJECT_BINARY_DIR}/${module}/include ${SUBMODULE_INCLUDES})
endforeach(module)

list(APPEND QUASAR_GLOBAL_INCLUDE_DIRECTORIES ${MODULE_INCLUDES})

add_library(quasar_project_options INTERFACE)
if(QUASAR_GLOBAL_COMPILE_DEFINITIONS)
    target_compile_definitions(quasar_project_options INTERFACE ${QUASAR_GLOBAL_COMPILE_DEFINITIONS})
endif()
if(QUASAR_GLOBAL_COMPILE_OPTIONS)
    target_compile_options(quasar_project_options INTERFACE ${QUASAR_GLOBAL_COMPILE_OPTIONS})
endif()
if(QUASAR_GLOBAL_INCLUDE_DIRECTORIES)
    target_include_directories(quasar_project_options INTERFACE ${QUASAR_GLOBAL_INCLUDE_DIRECTORIES})
endif()

# add all module directories and add module objects
set(MODULES_IN_SOURCE_DIR ${NATIVE_SERVER_MODULES} ${CUSTOM_SERVER_MODULES})
foreach(module ${MODULES_IN_SOURCE_DIR})
    add_subdirectory( ${module} )
    if(TARGET ${module})
        target_link_libraries(${module} PRIVATE quasar_project_options)
    endif()
    set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}>)
    add_dependencies( ${module} quasar_opcua_backend_is_ready )
endforeach(module)

foreach(module ${OPTIONAL_SERVER_MODULES})
    add_subdirectory( ${PROJECT_BINARY_DIR}/${module} ${PROJECT_BINARY_DIR}/${module} )  # listed twice, 1st is SOURCE_DIR 2nd is BINARY_DIR
    if(TARGET ${module})
        target_link_libraries(${module} PRIVATE quasar_project_options)
    endif()
    set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}>)
endforeach(module)

add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/Design/validated.tmp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/quasar.py validate_design --project_binary_dir ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/Design
    COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/Design/validated.tmp
    DEPENDS ${DESIGN_FILE}  ${PROJECT_SOURCE_DIR}/Design/Design.xsd
    )
add_custom_target(validateDesign DEPENDS  ${PROJECT_BINARY_DIR}/Design/validated.tmp )


set(QUASAR_TARGET_LINK_DIRECTORIES ${BOOST_PATH_LIBS} ${SERVER_LINK_DIRECTORIES})
if(DEFINED OPCUA_TOOLKIT_PATH AND NOT OPCUA_TOOLKIT_PATH STREQUAL "")
    list(APPEND QUASAR_TARGET_LINK_DIRECTORIES ${OPCUA_TOOLKIT_PATH}/lib)
endif()
list(REMOVE_DUPLICATES QUASAR_TARGET_LINK_DIRECTORIES)

add_executable ( ${EXECUTABLE}
	${MODULES_OBJECTS}
	)

# By default, Visual Studio will create subfolders for the binaries (Release, Debug),
# the code below overrides this and forces all binaries to be created in the same folder, which is bin/
if (MSVC)
    set_target_properties(${EXECUTABLE} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/bin"
    )
else()
    set_target_properties( ${EXECUTABLE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/ )
endif()

target_link_libraries(${EXECUTABLE} PRIVATE quasar_project_options)
if(QUASAR_TARGET_LINK_DIRECTORIES)
    target_link_directories(${EXECUTABLE} PRIVATE ${QUASAR_TARGET_LINK_DIRECTORIES})
endif()

find_package(Threads REQUIRED)


set(TARGET_LIBS
        ${BOOST_LIBS}
        ${OPCUATOOLKIT_LIBRARIES}
        XercesC::XercesC
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        LibXml2::LibXml2
        ${SERVER_LINK_LIBRARIES}
        )

option(ENABLE_QUASAR_SANITIZERS "Enable quasar sanitizers" OFF)
if(ENABLE_QUASAR_SANITIZERS)
    MESSAGE(STATUS "Enabling quasar sanitizers")
    include (${PROJECT_SOURCE_DIR}/cmake/QuasarSanitizers.cmake)
    enable_quasar_sanitizers(${EXECUTABLE})
endif()

message(STATUS "Target Link Libraries: ${TARGET_LIBS}")

target_link_libraries ( ${EXECUTABLE} PRIVATE ${TARGET_LIBS} )


## HACK TO REMOVE
if(MSVC)
  target_link_options(OpcUaServer PRIVATE
    /NODEFAULTLIB:libboost_thread-vc143-mt-s-x64-1_90.lib
    /NODEFAULTLIB:libboost_program_options-vc143-mt-s-x64-1_90.lib
    /NODEFAULTLIB:libboost_chrono-vc143-mt-s-x64-1_90.lib
  )
endif(MSVC)


if(BUILD_SERVER_SHARED_LIB)
        set( SERVER_SHARED_LIB_TARGET "${EXECUTABLE}Object" )
        add_library ( ${SERVER_SHARED_LIB_TARGET} SHARED ${MODULES_OBJECTS} )
        set_target_properties( ${SERVER_SHARED_LIB_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY bin/ )
        target_link_libraries(${SERVER_SHARED_LIB_TARGET} PRIVATE quasar_project_options)
        if(QUASAR_TARGET_LINK_DIRECTORIES)
            target_link_directories(${SERVER_SHARED_LIB_TARGET} PRIVATE ${QUASAR_TARGET_LINK_DIRECTORIES})
        endif()
	target_link_libraries( ${SERVER_SHARED_LIB_TARGET} PRIVATE ${TARGET_LIBS})
endif(BUILD_SERVER_SHARED_LIB)

if(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
	  MESSAGE("CMakeEpilogue found")
	  include (${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
endif(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
