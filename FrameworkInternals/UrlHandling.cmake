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

# Author: Stefan Schlenker

function ( process_git_url URL )

    # replace git server base URL if defined in project settings
    # example: setting GIT_SERVER_REPLACEMENT="ssh://gitlab.com:7999/"
    # and calling process_git_url(URL) with URL=https://github.com/quasar-team/MyQuasarModule.git
    # results in URL to be set to "ssh://gitlab.com:7999/quasar-team/MyQuasarModule.git"
    #
    if (DEFINED GIT_SERVER_REPLACEMENT)
      set( NEW_URL ${${URL}} )
      set( MATCHED )
      string(REGEX MATCH "(http|https|ssh):\\/\\/[-a-zA-Z0-9@:%._\\+~#=]+\\.[a-z]+(:[0-9]+)?\\/" MATCHED ${NEW_URL})
      message("process_git_url(): Matched ${MATCHED} in ${${URL}}")
      if ( MATCHED )
        string(REPLACE ${MATCHED} ${GIT_SERVER_REPLACEMENT} NEW_URL ${NEW_URL})
        message("process_git_url(): Replaced to ${NEW_URL}")
      endif()

      set( ${URL} ${NEW_URL} PARENT_SCOPE )
    endif()

endfunction ()
