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
cmake_minimum_required(VERSION 2.8)

message("PROJECT_SOURCE_DIR= ${PROJECT_SOURCE_DIR}")
message("PROJECT_BINARY_DIR= ${PROJECT_BINARY_DIR}")

add_definitions(
	-DSUPPORT_XML_CONFIG 
)

if(EXISTS ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake)
	  include( ${PROJECT_SOURCE_DIR}/FrameworkInternals/build_config_selector.cmake )
else()
     message(FATAL_ERROR "Build configuration has not been chosen yet. Please run 'quasar.py set_build_config'")
endif()

include( ProjectSettings.cmake )

set(PYTHON_COMMAND python)
set(DESIGN_FILE ${PROJECT_SOURCE_DIR}/Design/Design.xml)



include_directories (
	${OPCUA_TOOLKIT_PATH}/include/uastack
	${OPCUA_TOOLKIT_PATH}/include/uabase
 	${OPCUA_TOOLKIT_PATH}/include/uaserver
	${OPCUA_TOOLKIT_PATH}/include/xmlparser
	${OPCUA_TOOLKIT_PATH}/include/uapki
	${BOOST_PATH_HEADERS}
	${PROJECT_BINARY_DIR}/Device/generated
	${PROJECT_SOURCE_DIR}/Configuration  # this is to cover Configurator.h
	${PROJECT_BINARY_DIR}/Configuration
	${SERVER_INCLUDE_DIRECTORIES}
	)

#
# begin optional quasar modules
#
set(OPTIONAL_SERVER_MODULES) 
file(GLOB OPTIONAL_SERVER_MODULE_URLS "FrameworkInternals/EnabledModules/*.url")

foreach(moduleFile ${OPTIONAL_SERVER_MODULE_URLS})
    #
    file (STRINGS ${moduleFile} OPT_MODULE_URL)
    get_filename_component(module ${moduleFile} NAME_WE) 
    file (STRINGS FrameworkInternals/EnabledModules/${module}.tag OPT_MODULE_TAG)
    message("Adding module ${module} URL=${OPT_MODULE_URL} TAG=${OPT_MODULE_TAG}")
    set(OPTIONAL_SERVER_MODULES ${OPTIONAL_SERVER_MODULES} ${module})
    set(OPT_MODULE_NAME ${module})
    configure_file(${PROJECT_SOURCE_DIR}/FrameworkInternals/OptionalModule.cmake
                   FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake OUTPUT_FILE ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_download.log)
    if (EXISTS ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME})
       message("Module ${module} was added.")
       execute_process(COMMAND git remote set-url --push origin push-disabled WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME})
    else()
       message("Error, module ${module} was not properly fetched. Please check FrameworkInternals/EnabledModules/${module}_download.log for errors.")
    endif()
    #
endforeach(moduleFile)
#
# end quasar modules
#

set(NATIVE_SERVER_MODULES AddressSpace Configuration Common Server Device Meta LogIt) 
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
endforeach(module)

foreach(module ${OPTIONAL_SERVER_MODULES})    
    add_subdirectory( ${PROJECT_BINARY_DIR}/${module} ${PROJECT_BINARY_DIR}/${module} )  # listed twice, 1st is SOURCE_DIR 2nd is BINARY_DIR
    set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}>)
endforeach(module)




add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/Design/validated.tmp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/quasar.py validate_design --project_binary_dir ${PROJECT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_BINARY_DIR}/Design/validated.tmp
    DEPENDS ${DESIGN_FILE}   
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

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set ( OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_DEBUG} )
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set ( OPCUA_TOOLKIT_LIBS ${OPCUA_TOOLKIT_LIBS_RELEASE} )
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

target_link_libraries ( ${EXECUTABLE} 
	${BOOST_LIBS}
	${XML_LIBS}
	${OPCUA_TOOLKIT_LIBS}
	${SERVER_LINK_LIBRARIES}
)

if(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
	  MESSAGE("CMakeEpilogue found")
	  include (${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
endif(EXISTS ${PROJECT_SOURCE_DIR}/CMakeEpilogue.cmake)
