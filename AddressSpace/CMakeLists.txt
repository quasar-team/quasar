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
	include(${PROJECT_SOURCE_DIR}/AddressSpace/cmake_generated.cmake)
else(${QUASAR_IS_CMAKE_BUILD_ENTRY_POINT})
	include(${PROJECT_BINARY_DIR}/AddressSpace/cmake_generated.cmake)
endif(${QUASAR_IS_CMAKE_BUILD_ENTRY_POINT})

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/AddressSpace/include/ASInformationModel.h ${PROJECT_BINARY_DIR}/AddressSpace/src/ASInformationModel.cpp
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
COMMAND ${PYTHON_COMMAND} quasar.py generate info_model --project_binary_dir ${PROJECT_BINARY_DIR}
DEPENDS ${DESIGN_FILE} templates/designToInformationModelHeader.jinja templates/designToInformationModelBody.jinja ${QUASAR_TRANSFORM_SENSITIVE_FILES}
)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/AddressSpace/src/SourceVariables.cpp ${PROJECT_BINARY_DIR}/AddressSpace/include/SourceVariables.h
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
COMMAND ${PYTHON_COMMAND} quasar.py generate source_variables --project_binary_dir ${PROJECT_BINARY_DIR}
DEPENDS
	${DESIGN_FILE}
	${PROJECT_SOURCE_DIR}/AddressSpace/templates/designToSourceVariablesHeader.jinja
	${PROJECT_SOURCE_DIR}/AddressSpace/templates/designToSourceVariablesBody.jinja
	DeviceBase
)

add_library (AddressSpace OBJECT
    src/ASInformationModel.cpp
    src/ASNodeManager.cpp
    src/ASSourceVariableIoManager.cpp
    src/SourceVariables.cpp
    src/ArrayTools.cpp
    src/ChangeNotifyingVariable.cpp
    src/FreeVariablesEngine.cpp
    ${ADDRESSSPACE_CLASSES}

	)


add_custom_target(AddressSpaceGeneratedHeaders DEPENDS ${ADDRESSSPACE_HEADERS} include/ASInformationModel.h include/SourceVariables.h)
add_dependencies (AddressSpace AddressSpaceGeneratedHeaders DeviceGeneratedHeaders Configuration.hxx_GENERATED )
