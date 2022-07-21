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

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Configuration/Configuration.xsd
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_xsd --project_binary_dir ${PROJECT_BINARY_DIR}
	DEPENDS ${DESIGN_FILE} templates/designToConfigurationXSD.jinja validateDesign ${PROJECT_SOURCE_DIR}/quasar.py ${QUASAR_TRANSFORM_SENSITIVE_FILES}
	)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Configuration/Configuration.cxx ${PROJECT_BINARY_DIR}/Configuration/Configuration.hxx
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Configuration
	COMMAND xsdcxx cxx-tree --std c++11 --ordered-type-all --generate-serialization --namespace-map http://cern.ch/quasar/Configuration=Configuration --output-dir ${PROJECT_BINARY_DIR}/Configuration ${PROJECT_BINARY_DIR}/Configuration/Configuration.xsd
	DEPENDS ${PROJECT_BINARY_DIR}/Configuration/Configuration.xsd
)

add_custom_target(Configuration.hxx_GENERATED DEPENDS ${PROJECT_BINARY_DIR}/Configuration/Configuration.hxx )

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Configuration/Configurator.cpp
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_cpp --project_binary_dir ${PROJECT_BINARY_DIR}
	DEPENDS ${DESIGN_FILE} templates/designToConfigurator.jinja ${PROJECT_SOURCE_DIR}/quasar.py AddressSpace Device
)

add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/Configuration/ConfigValidator.cpp
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	COMMAND ${PYTHON_COMMAND} quasar.py generate config_validator --project_binary_dir ${PROJECT_BINARY_DIR}
	DEPENDS ${DESIGN_FILE} templates/designToConfigValidator.jinja ${PROJECT_SOURCE_DIR}/quasar.py AddressSpace Device
)

add_library (Configuration OBJECT
	Configuration.cxx
	Configuration.hxx
	Configurator.cpp
	ConfigValidator.cpp
	)
