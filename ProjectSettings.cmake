## ----------------------------------------------------
## Please see Documentation/quasarBuildSystem.html for
## information how to use this file.
## ----------------------------------------------------

set(CUSTOM_SERVER_MODULES )
set(EXECUTABLE OpcUaServer)
set(SERVER_INCLUDE_DIRECTORIES  )
set(SERVER_LINK_LIBRARIES  )
set(SERVER_LINK_DIRECTORIES  )

##
## Add here any additional boost libraries needed with their canonical name
## examples: date_time atomic etc.
## Note: boost paths are resolved either from $BOOST_ROOT if defined or system paths as fallback
##
set(ADDITIONAL_BOOST_LIBS )

##
## Add any other cmake instructions below which are specific to this project
## and should be executed as a prefix to quasar cmake instructions.
## Note that post-build instructions such as install targets
## should be defined in a file named CMakeEpilogue.cmake
##
