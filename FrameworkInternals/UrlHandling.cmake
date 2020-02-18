function ( process_git_url URL )

    # replace github base URL if defined in project settings
    if (DEFINED GITHUB_REPLACEMENT)
      set( NEW_URL ${${URL}} )
      string(REPLACE "https://github.com/" ${GITHUB_REPLACEMENT} NEW_URL ${NEW_URL})
      message("process_git_url(): Replaced ${${URL}} with ${NEW_URL}")
      set( ${URL} ${NEW_URL} PARENT_SCOPE )
    endif()

endfunction ()
