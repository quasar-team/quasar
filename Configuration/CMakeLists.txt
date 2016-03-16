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

MESSAGE( STATUS "Design file=" ${DESIGN_FILE} )

add_custom_command(OUTPUT Configuration.xsd
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_xsd
	DEPENDS ${DESIGN_FILE} designToConfigurationXSD.xslt validateDesign ${PROJECT_SOURCE_DIR}/quasar.py
	)

add_custom_command(OUTPUT Configuration.cxx Configuration.hxx
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Configuration
	COMMAND xsdcxx cxx-tree  --generate-serialization --namespace-map http://cern.ch/quasar/Configuration=Configuration  Configuration.xsd
	DEPENDS Configuration.xsd
)

add_custom_target(Configuration.hxx DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Configuration.hxx)

add_custom_command(OUTPUT Configurator.cpp	
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_cpp
	DEPENDS ${DESIGN_FILE} designToConfigurator.xslt ${PROJECT_SOURCE_DIR}/quasar.py AddressSpace Device
)

add_custom_command(OUTPUT ConfigValidator.cpp	
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_validator
	DEPENDS ${DESIGN_FILE} designToConfigValidator.xslt ${PROJECT_SOURCE_DIR}/quasar.py AddressSpace Device
)

add_library (Configuration OBJECT
	Configuration.cxx
	Configuration.hxx
	Configurator.cpp
	ConfigValidator.cpp
	)
