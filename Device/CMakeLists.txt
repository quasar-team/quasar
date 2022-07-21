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

# QUASAR_IS_CMAKE_BUILD_ENTRY_POINT should evaluate to true whenever plain CMake is the entry point, like in Yocto
if(${QUASAR_IS_CMAKE_BUILD_ENTRY_POINT})
    set(GENERATED_DEVICE_CMAKE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Device/generated)
else(${QUASAR_IS_CMAKE_BUILD_ENTRY_POINT})
    set(GENERATED_DEVICE_CMAKE_INCLUDE_DIR ${PROJECT_BINARY_DIR}/Device/generated)
endif(${QUASAR_IS_CMAKE_BUILD_ENTRY_POINT})
include( ${GENERATED_DEVICE_CMAKE_INCLUDE_DIR}/cmake_device_base_header.cmake )
include( ${GENERATED_DEVICE_CMAKE_INCLUDE_DIR}/cmake_device_header.cmake )
message( STATUS "DEVICEBASE_GENERATED_FILES [${DEVICEBASE_GENERATED_FILES}]")
message( STATUS "DEVICE_CLASSES [${DEVICE_CLASSES}]")
    
# check quasar/Device/DeviceCustom.cmake.example for an example!
if(EXISTS ${PROJECT_SOURCE_DIR}/Device/DeviceCustom.cmake)
  include (DeviceCustom.cmake)
endif(EXISTS ${PROJECT_SOURCE_DIR}/Device/DeviceCustom.cmake)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Device/include/DRoot.h ${PROJECT_BINARY_DIR}/Device/src/DRoot.cpp
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
COMMAND ${PYTHON_COMMAND} quasar.py generate root --project_binary_dir ${PROJECT_BINARY_DIR}
DEPENDS ${DESIGN_FILE}
    ${PROJECT_SOURCE_DIR}/quasar.py
    validateDesign
    templates/designToRootHeader.jinja
    templates/designToRootBody.jinja
    templates/commonDeviceTemplates.jinja
    ${QUASAR_TRANSFORM_SENSITIVE_FILES}
)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Device/generated/Base_All.cpp
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
COMMAND ${PYTHON_COMMAND} quasar.py generate base_cpp_all --project_binary_dir ${PROJECT_BINARY_DIR}
DEPENDS
  ${DESIGN_FILE}
  ${PROJECT_SOURCE_DIR}/quasar.py
  Configuration.hxx_GENERATED
  validateDesign
  templates/commonDeviceTemplates.jinja
  ${QUASAR_TRANSFORM_SENSITIVE_FILES}
  templates/designToDeviceBaseBody.jinja
)

add_custom_target(DeviceBase DEPENDS ${DEVICEBASE_GENERATED_FILES} )

add_custom_target(DeviceGeneratedHeaders DEPENDS include/DRoot.h ${DEVICEBASE_GENERATED_FILES})


message( STATUS "DEVICEBASE_GENERATED_FILES [${DEVICEBASE_GENERATED_FILES}]")
message( STATUS "DEVICE_CLASSES [${DEVICE_CLASSES}]")
add_library (Device OBJECT

    ${DEVICEBASE_GENERATED_FILES} # generated from transform
    ${DEVICE_CLASSES} # generated from transform
    ${DEVICE_CUSTOM_SOURCES} # if used, would come from DeviceCustom.cmake

    # Your custom Device logic code - Note:
    # since quasar 1.5.x / nebula.B2 you *MUST* list such files in DeviceCustom.cmake
    # rather than in here. This file will be overwritten each time you upgrade your quasar project,
    # so you will lose your changes.

)

add_dependencies (Device Configuration.hxx_GENERATED AddressSpaceGeneratedHeaders DeviceBase )
