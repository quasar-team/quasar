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

# Author: Stefan Schlenker
# Author: Piotr Nikiel <piotr@nikiel.info>

file(GLOB OPTIONAL_SERVER_MODULE_URLS "FrameworkInternals/EnabledModules/*.url")

include( FrameworkInternals/UrlHandling.cmake )
foreach(moduleFile ${OPTIONAL_SERVER_MODULE_URLS})
    #
    file (STRINGS ${moduleFile} OPT_MODULE_URL)

    process_git_url(OPT_MODULE_URL)

    get_filename_component(module ${moduleFile} NAME_WE)
    file (STRINGS FrameworkInternals/EnabledModules/${module}.tag OPT_MODULE_TAG)
    message("Adding module ${module} URL=${OPT_MODULE_URL} TAG=${OPT_MODULE_TAG}")
    set(OPTIONAL_SERVER_MODULES ${OPTIONAL_SERVER_MODULES} ${module})
    set(OPT_MODULE_NAME ${module})
    configure_file(${PROJECT_SOURCE_DIR}/FrameworkInternals/OptionalModule.cmake
                   FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_cmake OUTPUT_FILE ${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_download.log)
    if (EXISTS ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME})
       message("Module ${module} was added.")
       execute_process(COMMAND git remote set-url --push origin push-disabled WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME})
    else()
       message("Error, module ${module} was not properly fetched. Please check FrameworkInternals/EnabledModules/${module}_download.log for errors.")
    endif()
    #
endforeach(moduleFile)
