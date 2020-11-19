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

project(opc_ua)
cmake_minimum_required(VERSION 3.10)
# the line below is added to quasar to keep backwards compatibility for the transition period.
cmake_policy(SET CMP0065 OLD)

if(NOT ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
message("Building out-of-source")
message("PROJECT_SOURCE_DIR= ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR= ${PROJECT_BINARY_DIR}")
endif()

add_definitions(
	-DSUPPORT_XML_CONFIG
	-DNOMINMAX  # doesn't include windows' min() and max() which break std c++ code. Should be neutral to non-windows code.
)

if(EXISTS ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake)
	  include( ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake )
else()
     message(FATAL_ERROR "Build configuration has not been chosen yet. Please run 'quasar.py set_build_config'")
endif()

include( ProjectSettings.cmake )

if( ( NOT DEFINED IGNORE_DEFAULT_BOOST_SETUP ) OR ( IGNORE_DEFAULT_BOOST_SETUP STREQUAL "OFF" ) )
    message( STATUS "Setting up boost location and libraries using FrameworkInternals/BoostSetup.cmake")
    include( FrameworkInternals/BoostSetup.cmake )
else()
    message( STATUS "Automatic boost setup was set to be ignored (IGNORE_DEFAULT_BOOST_SETUP). Boost libs and includes must be defined elsewhere (e.g. in the build config)." )
    include_directories(${BOOST_PATH_HEADERS})  # this falls back to the old solution when we didn't yet use find_package(Boost)
endif()

set(PYTHON_COMMAND python3)
set(DESIGN_FILE ${PROJECT_SOURCE_DIR}/Design/Design.xml)

include_directories (
	${PROJECT_BINARY_DIR}/Device/generated
	${PROJECT_SOURCE_DIR}/Configuration  # this is to cover Configurator.h
	${PROJECT_BINARY_DIR}/Configuration
	${SERVER_INCLUDE_DIRECTORIES}
	)

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set ( OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_DEBUG} )
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set ( OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_RELEASE} )
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

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
foreach(include ${MODULE_INCLUDES})
    include_directories(${include})
endforeach(include)

# add all module directories and add module objects
set(MODULES_IN_SOURCE_DIR ${NATIVE_SERVER_MODULES} ${CUSTOM_SERVER_MODULES})
foreach(module ${MODULES_IN_SOURCE_DIR})
    add_subdirectory( ${module} )
    set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}>)
    add_dependencies( ${module} quasar_opcua_backend_is_ready )
endforeach(module)

foreach(module ${OPTIONAL_SERVER_MODULES})
    add_subdirectory( ${PROJECT_BINARY_DIR}/${module} ${PROJECT_BINARY_DIR}/${module} )  # listed twice, 1st is SOURCE_DIR 2nd is BINARY_DIR
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

add_custom_target(dep ALL
        COMMAND ${PROJECT_SOURCE_DIR}/quasar.py design_vs_device
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        )

link_directories(
	${OPCUA_TOOLKIT_PATH}/lib
	${BOOST_PATH_LIBS}
	${SERVER_LINK_DIRECTORIES}
)
add_executable ( ${EXECUTABLE}
	${MODULES_OBJECTS}
	)
set_target_properties( ${EXECUTABLE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/ )

set(TARGET_LIBS
        ${BOOST_LIBS}
        ${XML_LIBS}
        ${OPCUA_TOOLKIT_LIBS}
        ${QUASAR_SERVER_LIBS}
        ${SERVER_LINK_LIBRARIES}
        )

target_link_libraries ( ${EXECUTABLE} ${TARGET_LIBS} )

if(BUILD_SERVER_SHARED_LIB)
        set( SERVER_SHARED_LIB_TARGET "${EXECUTABLE}Object" )
        add_library ( ${SERVER_SHARED_LIB_TARGET} SHARED ${MODULES_OBJECTS} )
        set_target_properties( ${SERVER_SHARED_LIB_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY bin/ )
	target_link_libraries( ${SERVER_SHARED_LIB_TARGET} ${TARGET_LIBS})
endif(BUILD_SERVER_SHARED_LIB)

if(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
	  MESSAGE("CMakeEpilogue found")
	  include (${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
endif(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
