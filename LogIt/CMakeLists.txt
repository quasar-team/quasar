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

if( NOT DEFINED LOGIT_HAS_BOOSTLOG )
	set (LOGIT_HAS_BOOSTLOG FALSE)
endif()

if( NOT DEFINED LOGIT_HAS_STDOUTLOG )
	set (LOGIT_HAS_STDOUTLOG TRUE)
endif()

if( NOT DEFINED LOGIT_HAS_UATRACE )
	set (LOGIT_HAS_UATRACE TRUE)
endif()
MESSAGE( STATUS "LogIt build options: LOGIT_HAS_STDOUTLOG [${LOGIT_HAS_STDOUTLOG}] LOGIT_HAS_BOOSTLOG [${LOGIT_HAS_BOOSTLOG}] LOGIT_HAS_UATRACE [${LOGIT_HAS_UATRACE}]" )

set(LOGIT_SOURCES
    src/LogIt.cpp
    src/LogLevels.cpp
    src/LogRecord.cpp
    src/LogSinks.cpp
    src/ComponentAttributes.cpp
    src/LogItInstance.cpp
)    

if(${LOGIT_HAS_BOOSTLOG})
    add_definitions(-DLOGIT_HAS_BOOSTLOG)
    set(LOGIT_SOURCES ${LOGIT_SOURCES} src/BoostRotatingFileLog.cpp)
endif(${LOGIT_HAS_BOOSTLOG})  

if(${LOGIT_HAS_STDOUTLOG})
    add_definitions(-DLOGIT_HAS_STDOUTLOG)
    set(LOGIT_SOURCES ${LOGIT_SOURCES} src/StdOutLog.cpp)
endif(${LOGIT_HAS_STDOUTLOG})

if(${LOGIT_HAS_UATRACE})
    add_definitions(-DLOGIT_HAS_UATRACE)
    set(LOGIT_SOURCES ${LOGIT_SOURCES} src/UaTraceSink.cpp)
endif(${LOGIT_HAS_UATRACE})

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

MESSAGE( STATUS "building LogIt from source files [${LOGIT_SOURCES}]" )
add_library (LogIt OBJECT ${LOGIT_SOURCES})
