# @author pnikiel
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.

message("No build configuration was specified. Assuming defaults from default_configuration.cmake")

#-------
#Boost
#-------
#We would prefer to use find_package for Boost but unfortunately this fails for SLC6.
#So we try environment variables, and if that fails, try sth really standard.
if(DEFINED ENV{BOOST_PATH_HEADERS})
    message ("BOOST_PATH_HEADERS is in your environment, taking it into account")
    if((NOT DEFINED ENV{BOOST_PATH_LIBS}) OR (NOT DEFINED ENV{BOOST_LIBS}))
        message (FATAL_ERROR "You have BOOST_PATH_HEADERS in your environment, but you are missing one of this: BOOST_PATH_LIBS BOOST_LIBS    Please fix it or use custom build configuration file ")
    endif((NOT DEFINED ENV{BOOST_PATH_LIBS}) OR (NOT DEFINED ENV{BOOST_LIBS}))
    SET( BOOST_PATH_HEADERS $ENV{BOOST_PATH_HEADERS} )
    SET( BOOST_PATH_LIBS $ENV{BOOST_PATH_LIBS} )
    SET( BOOST_LIBS $ENV{BOOST_LIBS} )
else(DEFINED ENV{BOOST_PATH_HEADERS})   
    message ("Assuming standard Boost installation as no BOOST_PATH_HEADERS is defined in your environment")
    SET( BOOST_PATH_HEADERS "" )
    SET( BOOST_PATH_LIBS "" )
    SET( BOOST_LIBS "-lboost_program_options -lboost_thread" )
endif(DEFINED ENV{BOOST_PATH_HEADERS})

message ("using BOOST_LIBS [$ENV{BOOST_LIBS}]")

#------
#OPCUA
#------

# No OPC-UA Toolkit: using Open62541-compat instead. It is referenced in BACKEND_MODULES below
SET( OPCUA_TOOLKIT_PATH "" )
SET( OPCUA_TOOLKIT_LIBS_DEBUG "" )
SET( OPCUA_TOOLKIT_LIBS_RELEASE "" )

add_definitions( -DBACKEND_OPEN62541 )

#-----
#XML Libs
#-----
#As of 03-Sep-2015 I see no FindXerces or whatever in our Cmake 2.8 installation, so no find_package can be user...
# TODO perhaps also take it from environment if requested
SET( XML_LIBS "-lxerces-c" ) 

#-----
#General settings
#-----

# TODO: split between Win / MSVC, perhaps MSVC has different notation for these
add_definitions(-Wall -Wno-deprecated -std=gnu++0x -Wno-literal-suffix) 

# 62xxx no uatrace
set (LOGIT_HAS_UATRACE FALSE)

set(BACKEND_SERVER_MODULES open62541 open62541-compat)
