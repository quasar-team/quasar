# @author pnikiel
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

#-----
#General settings
#-----

add_definitions(-Wall -Wno-deprecated) 

# open62541-compat has no uatrace
set (LOGIT_HAS_UATRACE FALSE)

# need C++11
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x" )

#-------
#Boost
#-------
# In a well configured Yocto project this should be OK
SET( BOOST_LIBS "-lboost_program_options-mt -lboost_thread-mt -lboost_system-mt" )

message ("using BOOST_LIBS [${BOOST_LIBS}]")

#------
#OPCUA
#------

# No OPC-UA Toolkit: using Open62541-compat instead. It is referenced in BACKEND_MODULES below
add_definitions( -DBACKEND_OPEN62541 )
SET( OPCUA_TOOLKIT_PATH "" )
SET( OPCUA_TOOLKIT_LIBS_RELEASE "${PROJECT_BINARY_DIR}/open62541-compat/open62541/libopen62541.a" -lrt)
SET( OPCUA_TOOLKIT_LIBS_DEBUG "${PROJECT_BINARY_DIR}/open62541-compat/open62541/libopen62541.a" -lrt)
include_directories( ${PROJECT_BINARY_DIR}/open62541-compat/open62541 )

#-----
#XML Libs
#-----
#As of 03-Sep-2015 I see no FindXerces or whatever in our Cmake 2.8 installation, so no find_package can be user...
# TODO perhaps also take it from environment if requested
SET( XML_LIBS "-lxerces-c -lssl" ) 

