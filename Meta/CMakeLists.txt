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

# Author: Benjamin Farnham <benjamin.farnham@cern.ch>

project(Meta CXX)

file(GLOB META_SRCS src/*.cpp)

add_library (Meta OBJECT ${META_SRCS})

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/generated)

add_custom_target(runBuildInfoGeneratorPy ALL
	DEPENDS alwaysRunBuildInfoGeneratorPy
)

add_custom_command(OUTPUT alwaysRunBuildInfoGeneratorPy # note output never actually created: ensures cmd always runs
	COMMENT "generating meta build info definitions header using python script" 
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/meta_build_info.py
	COMMAND python ${CMAKE_CURRENT_LIST_DIR}/meta_build_info.py --target_generation_dir ${PROJECT_BINARY_DIR}/generated --commit_ID "TODO" --toolkit_libs "${OPCUA_TOOLKIT_LIBS} "
)

add_dependencies (Meta Configuration.hxx_GENERATED AddressSpace runBuildInfoGeneratorPy)
