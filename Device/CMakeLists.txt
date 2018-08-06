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

include(generated/cmake_header.cmake)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Device/include/DRoot.h ${PROJECT_BINARY_DIR}/Device/src/DRoot.cpp
WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
COMMAND python quasar.py generate root --project_binary_dir ${PROJECT_BINARY_DIR}
DEPENDS ${DESIGN_FILE} ${PROJECT_SOURCE_DIR}/quasar.py validateDesign designToRootHeader.xslt designToRootBody.xslt
)

add_library (Device OBJECT

    ${DEVICEBASE_GENERATED_FILES}
    ${DEVICE_CLASSES}
    
    # Your custom Device logic code - begin. Skip classes! (They are declared
    # in ${DEVICE_CLASSES) 
    
	
    # Your custom Device logic code - end
	
)
    
add_dependencies (Device Configuration.hxx_GENERATED AddressSpaceGeneratedHeaders DeviceBase )

