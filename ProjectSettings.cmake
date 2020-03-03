## ----------------------------------------------------
## Please see Documentation/quasarBuildSystem.html for
## information how to use this file.
## ----------------------------------------------------

set(CUSTOM_SERVER_MODULES )
set(EXECUTABLE OpcUaServer)
set(SERVER_INCLUDE_DIRECTORIES  )
set(SERVER_LINK_LIBRARIES  )
set(SERVER_LINK_DIRECTORIES  )

## -------
##  Boost
## -------
## Example for adding additional boost libraries.
## Edit and uncomment if required
##
#set( BOOST_LIBS ${BOOST_LIBS} "-lboost_filesystem-mt -lboost_program_options-mt -lboost_date_time-mt" )

##
## Add any other cmake instructions below which are specific to this project
##