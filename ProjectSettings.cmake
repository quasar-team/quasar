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
##  Shared object
## -------
## If ON, in addition to an executable, a shared object will be created.
##
set(BUILD_SERVER_SHARED_LIB OFF)

## -------
##  Github replacement
## -------
## If used, replaces https://github.com/ in modules which are fetched on demand,
## this can be used if private mirror git servers are used instead of github.
## Example, edit and uncomment if required.
#
#set(GITHUB_REPLACEMENT "ssh://git@gitlab.cern.ch:7999/" )

## -------
##  Boost
## -------
## Example for adding additional boost libraries.
## Edit and uncomment if required
##
#set( BOOST_LIBS ${BOOST_LIBS} "-lboost_filesystem-mt -lboost_program_options-mt -lboost_date_time-mt" )

## -------
##  LogIt
## -------
## Optional switches for logging back-ends to be used.
## Example for logging to stdout only, edit and uncomment if required.
##
#set(LOGIT_BACKEND_STDOUTLOG ON CACHE BOOL "The basic back-end: logs to stdout")
#set(LOGIT_BACKEND_BOOSTLOG OFF CACHE BOOL "Rotating file logger back-end: fixed size on disk based on boost logging library")
#set(LOGIT_BACKEND_UATRACE OFF CACHE BOOL "UnifiedAutomation toolkit logger")

##
## Add any other build related cmake instructions below
##