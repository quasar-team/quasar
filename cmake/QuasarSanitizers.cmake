# LICENSE:
# Copyright (c) 2023, CERN
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

# Author: Paris Moschovakos <paris.moschovakos@cern.ch>

function(enable_quasar_sanitizers executable_name)

    message(STATUS "Enabling sanitizers for target: ${executable_name}")

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

        if(ENABLE_COVERAGE)
            add_compile_options(--coverage -O0 -g)
            target_link_libraries(${executable_name} --coverage)
        endif()

        set(SANITIZERS "")

        option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)

        if(ENABLE_SANITIZER_ADDRESS)
            MESSAGE(STATUS "Enabling address sanitizer")
            list(APPEND SANITIZERS "address")
        endif()

        option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)

        if(ENABLE_SANITIZER_LEAK)
            MESSAGE(STATUS "Enabling leak sanitizer")
            list(APPEND SANITIZERS "leak")
        endif()

        option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" OFF)

        if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
            MESSAGE(STATUS "Enabling undefined behavior sanitizer")
            list(APPEND SANITIZERS "undefined")
        endif()

        option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)

        if(ENABLE_SANITIZER_THREAD)
            if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
                MESSAGE(WARNING "Thread sanitizer does not work with Address and Leak sanitizer enabled")
            else()
                MESSAGE(STATUS "Enabling thread sanitizer")
                list(APPEND SANITIZERS "thread")
            endif()
        endif()

        option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)

        if(ENABLE_SANITIZER_MEMORY AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
            if("address" IN_LIST SANITIZERS
                OR "thread" IN_LIST SANITIZERS
                OR "leak" IN_LIST SANITIZERS)
                MESSAGE(WARNING "Memory sanitizer does not work with Address, Thread and Leak sanitizer enabled")
            else()
                MESSAGE(STATUS "Enabling memory sanitizer")
                list(APPEND SANITIZERS "memory")
            endif()
        endif()

        list(
            JOIN
            SANITIZERS
            ","
            LIST_OF_SANITIZERS)
    endif()

    if(LIST_OF_SANITIZERS)
        if(NOT "${LIST_OF_SANITIZERS}" STREQUAL "")
            add_compile_options(-fsanitize=${LIST_OF_SANITIZERS})
            target_link_libraries(${executable_name} -fsanitize=${LIST_OF_SANITIZERS})
        endif()
    endif()
endfunction()
