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
        string(REPLACE ${MATCHED} ${GITSERVER_REPLACEMENT} NEW_URL ${NEW_URL})
        message("process_git_url(): Replaced to ${NEW_URL}")
      endif()

      set( ${URL} ${NEW_URL} PARENT_SCOPE )
    endif()

endfunction ()
