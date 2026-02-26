# LICENSE:
# Copyright (c) 2020, CERN
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
# Author: Paris Moschovakos
# Author: Stefan Schlenker

#
# automatically set up boost with libs needed by quasar
# and optionally added ones for the project via ADDITIONAL_BOOST_LIBS
#
if(NOT DEFINED BOOST_LIBS)
    if(NOT DEFINED BOOST_ROOT AND DEFINED ENV{BOOST_HOME} AND NOT "$ENV{BOOST_HOME}" STREQUAL "")
        set(BOOST_ROOT "$ENV{BOOST_HOME}")
        message(STATUS "Using BOOST_HOME as BOOST_ROOT: [${BOOST_ROOT}]")
    endif()

    if (DEFINED ENV{BOOST_HOME} AND NOT "$ENV{BOOST_HOME}" STREQUAL "")
        set(BOOST_HOME "$ENV{BOOST_HOME}")
        list(APPEND CMAKE_PREFIX_PATH "${BOOST_HOME}")
        set(Boost_USE_STATIC_RUNTIME ON)
        set(Boost_USE_STATIC_LIBS ON)
        message(STATUS "Using BOOST_HOME environment variable: [${BOOST_HOME}]")
        message(STATUS "This will set BOOST to that folder in static mode")
    endif()

    set(_QUASAR_BOOST_COMPONENTS regex chrono program_options thread log ${ADDITIONAL_BOOST_LIBS})
    find_package(Boost CONFIG REQUIRED COMPONENTS ${_QUASAR_BOOST_COMPONENTS})
    if(NOT Boost_FOUND)
        message(FATAL_ERROR "Failed to find boost installation.")
    else()
        message(STATUS "Found system boost, version [${Boost_VERSION}], include dir [${Boost_INCLUDE_DIRS}]")
        include_directories(${Boost_INCLUDE_DIRS})
        set(BOOST_LIBS
            Boost::regex
            Boost::chrono
            Boost::program_options
            Boost::thread
            Boost::log)
        foreach(BOOST_COMPONENT IN LISTS ADDITIONAL_BOOST_LIBS)
            list(APPEND BOOST_LIBS Boost::${BOOST_COMPONENT})
        endforeach()
    endif()
endif()
