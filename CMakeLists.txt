# LICENSE:
# Copyright (c) 2015, CERN, University of Oviedo
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
add_definitions(
	-DSUPPORT_XML_CONFIG 
)

if(DEFINED CMAKE_TOOLCHAIN_FILE)
	message("CMAKE_TOOLCHAIN_FILE defined -- now including it")
	include(${CMAKE_TOOLCHAIN_FILE})
else(DEFINED CMAKE_TOOLCHAIN_FILE)
	message("CMAKE_TOOLCHAIN_FILE not defined -- loading default, FrameworkInternals/default_configuration.cmake")
	include( FrameworkInternals/default_configuration.cmake )
endif(DEFINED CMAKE_TOOLCHAIN_FILE)

set(PYTHON_COMMAND python)
set(EXECUTABLE OpcUaServer)

include_directories (
	${OPCUA_TOOLKIT_PATH}/include/uastack
	${OPCUA_TOOLKIT_PATH}/include/uabase
	${OPCUA_TOOLKIT_PATH}/include/uaserver
	${OPCUA_TOOLKIT_PATH}/include/xmlparser
	${OPCUA_TOOLKIT_PATH}/include/uapki
	${BOOST_PATH_HEADERS}
	${PROJECT_SOURCE_DIR}/Device/generated
	${PROJECT_SOURCE_DIR}/Configuration
	)

#
# begin optional quasar modules
#
set(OPTIONAL_SERVER_MODULES) 
file(GLOB OPTIONAL_SERVER_MODULE_URLS "FrameworkInternals/EnabledModules/*.url")

foreach(moduleFile ${OPTIONAL_SERVER_MODULE_URLS})
#foreach(module ${OPTIONAL_SERVER_MODULES})
    #
    file (STRINGS ${moduleFile} OPT_MODULE_URL)
    get_filename_component(module ${moduleFile} NAME_WE) 
    #string(REPLACE ".url" "" moduleName ${moduleFile})
    message("module=${module} URL=${OPT_MODULE_URL}")
    set(OPTIONAL_SERVER_MODULES ${OPTIONAL_SERVER_MODULES} ${module})

    set(OPT_MODULE_NAME ${module})
    message("adding ${module}")
    configure_file(${PROJECT_SOURCE_DIR}/OptionalModule.cmake
                   ${OPT_MODULE_NAME}_cmake/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME}_cmake)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME}_cmake )
    
    include_directories(${module}/include)
    #
#endforeach(module)
endforeach(moduleFile)
#
# end quasar modules
#

set(NATIVE_SERVER_MODULES AddressSpace Configuration Common Server Device Meta LogIt) 
set(SERVER_MODULES ${NATIVE_SERVER_MODULES} ${OPTIONAL_SERVER_MODULES})

foreach(module ${SERVER_MODULES})
	message("Adding headers for module: ${module}")
	include_directories(${PROJECT_SOURCE_DIR}/${module}/include )
endforeach(module)

set(MODULES_OBJECTS "")

foreach(module ${SERVER_MODULES})
	add_subdirectory( ${module} )
	set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}> )
endforeach(module)

set(DESIGN_FILE ${PROJECT_SOURCE_DIR}/Design/Design.xml)

add_custom_command(
    OUTPUT ${PROJECT_SOURCE_DIR}/Design/validated.tmp
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Design
    COMMAND ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/quasar.py validate_design
    COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/Design/validated.tmp
    DEPENDS ${DESIGN_FILE}   
    )
add_custom_target(validateDesign DEPENDS  ${PROJECT_SOURCE_DIR}/Design/validated.tmp )

add_custom_target(dep ALL COMMAND ${PROJECT_SOURCE_DIR}/quasar.py design_vs_device ) 


#foreach(module ${EXTERNAL_MODULES})
#	add_subdirectory( ../${module} )
#	set(MODULES_OBJECTS ${MODULES_OBJECTS} $<TARGET_OBJECTS:${module}> )
#endforeach(module)
	
link_directories(
	${OPCUA_TOOLKIT_PATH}/lib
	${BOOST_PATH_LIBS}
)
add_executable ( ${EXECUTABLE} 
	${MODULES_OBJECTS}
	)
set_target_properties( ${EXECUTABLE} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/ )

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_link_libraries ( ${EXECUTABLE} ${OPCUA_TOOLKIT_LIBS_DEBUG} )
else(${CMAKE_BUILD_TYPE} MATCHES Debug)
    target_link_libraries ( ${EXECUTABLE} ${OPCUA_TOOLKIT_LIBS_RELEASE} )
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

target_link_libraries ( ${EXECUTABLE} 
	${BOOST_LIBS}
	${XML_LIBS}

	# Add your custom libraries here
)


