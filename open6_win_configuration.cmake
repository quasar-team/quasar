# @author Damian Abalo
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.

message("using build configuration from enice_win_configuration.cmake")
message(STATUS "environment vars: BOOST_HOME [$ENV{BOOST_HOME}] CODE_SYNTHESYS_XSD [$ENV{CODE_SYNTHESYS_XSD}] OPENSSL [$ENV{OPENSSL}] XERCESC [$ENV{XERCESC}] LIBXML2[$ENV{LIBXML2}]")


#-------
#Boost
#-------
message(STATUS "starting the quest for boost, relevant environment variables: BOOST_PATH_HEADERS [$ENV{BOOST_PATH_HEADERS}] BOOST_PATH_LIBS [$ENV{BOOST_PATH_LIBS}] BOOST_HOME [$ENV{BOOST_HOME}]")

# headers path
if(DEFINED ENV{BOOST_PATH_HEADERS})
	SET( BOOST_PATH_HEADERS $ENV{BOOST_PATH_HEADERS} )
	message(STATUS "using BOOST_PATH_HEADERS from environment BOOST_PATH_HEADERS [$BOOST_PATH_HEADERS]")
else()
	if( DEFINED ENV{BOOST_HOME} )
		SET( BOOST_PATH_HEADERS $ENV{BOOST_HOME}/include )
		message(STATUS "using BOOST_PATH_HEADERS from environment BOOST_HOME [$BOOST_HOME] -> BOOST_PATH_HEADERS [$BOOST_PATH_HEADERS]")
	endif()
endif()

# libs path
if(DEFINED ENV{BOOST_PATH_LIBS})
	SET( BOOST_PATH_LIBS $ENV{BOOST_PATH_LIBS} )
	message(STATUS "using BOOST_PATH_LIBS from environment BOOST_PATH_LIBS [$BOOST_PATH_LIBS]")
else()
	if( DEFINED ENV{BOOST_HOME} )
		SET( BOOST_PATH_LIBS $ENV{BOOST_HOME}/lib )
		message(STATUS "using BOOST_PATH_LIBS from environment BOOST_HOME [$BOOST_HOME] -> BOOST_PATH_LIBS [$BOOST_PATH_LIBS]")
	endif()
endif()

# final check
if( NOT BOOST_PATH_HEADERS OR NOT BOOST_PATH_LIBS )
	message( FATAL_ERROR "unable to determine boost headers and library paths from environment variables BOOST_PATH_HEADERS [$ENV{BOOST_PATH_HEADERS}] BOOST_PATH_LIBS [$ENV{BOOST_PATH_LIBS}] BOOST_HOME [$ENV{BOOST_HOME}]")
endif()

message(STATUS "BOOST - include [${BOOST_PATH_HEADERS}] libs [${BOOST_PATH_LIBS}]")



if(NOT TARGET libboostprogramoptions)
	add_library(libboostprogramoptions STATIC IMPORTED)
	#libboost_program_options-vc141-mt-gd-x64-1_67.lib
	set_property(TARGET libboostprogramoptions PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_program_options-vc141/lib/native/libboost_program_options-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostsystem)
	add_library(libboostsystem STATIC IMPORTED)
	set_property(TARGET libboostsystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_system-vc141/lib/native/libboost_system-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostfilesystem)
	add_library(libboostfilesystem STATIC IMPORTED)
	set_property(TARGET libboostfilesystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_filesystem-vc141/lib/native/libboost_filesystem-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostchrono)
	add_library(libboostchrono STATIC IMPORTED)
	set_property(TARGET libboostchrono PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_chrono-vc141/lib/native/libboost_chrono-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostdatetime)
	add_library(libboostdatetime STATIC IMPORTED)
	set_property(TARGET libboostdatetime PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_date_time-vc141/lib/native/libboost_date_time-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostthread)
	add_library(libboostthread STATIC IMPORTED)
	set_property(TARGET libboostthread PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_thread-vc141/lib/native/libboost_thread-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostlog)
	add_library(libboostlog STATIC IMPORTED)
	set_property(TARGET libboostlog PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_log-vc141/lib/native/libboost_log-vc141-mt-x64-1_67.lib)
endif()
if(NOT TARGET libboostlogsetup)
	add_library(libboostlogsetup STATIC IMPORTED)
	set_property(TARGET libboostlogsetup PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/boost_log_setup-vc141/lib/native/libboost_log_setup-vc141-mt-x64-1_67.lib)
endif()

#set( BOOST_LIBS libboost_system-vc141-mt-x64-1_57 libboost_program_options-vc141-mt-x64-1_57 libboost_thread-vc141-mt-x64-1_57 libboost_date_time-vc141-mt-x64-1_57 libboost_chrono-vc141-mt-x64-1_57 libboost_regex-vc141-mt-x64-1_57 -lrt)
set( BOOST_LIBS  libboostlogsetup libboostlog libboostsystem libboostfilesystem libboostthread libboostprogramoptions libboostchrono libboostdatetime -lrt)

#-----
# LogIt
#-----
SET( LOGIT_HAS_STDOUTLOG TRUE )
SET( LOGIT_HAS_BOOSTLOG FALSE )
SET( LOGIT_HAS_UATRACE FALSE )
MESSAGE( STATUS "LogIt build options: stdout [${LOGIT_HAS_STDOUTLOG}] boost [${LOGIT_HAS_BOOSTLOG}] uaTrace [${LOGIT_HAS_UATRACE}]" )

#-----
#CodeSynthesys XSD
#-----
if( NOT DEFINED ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS} )
	message( FATAL_ERROR "unable to determine Code Synthesis headers from environment variables CODE_SYNTHESYS_XSD_PATH_HEADERS [$ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS}]")
else()
	message(STATUS "Using environment variable for CODE_SYNTHESYS_XSD_PATH_HEADERS [$ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS}")
endif()
include_directories($ENV{CODE_SYNTHESYS_XSD_PATH_HEADERS})

#----
#OPENSSL
#----
if( NOT DEFINED ENV{OPENSSL_PATH_HEADERS} OR NOT DEFINED ENV{OPENSSL_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine openssl headers and library paths from environment variables OPENSSL_PATH_HEADERS [$ENV{OPENSSL_PATH_HEADERS}] OPENSSL_PATH_LIBS [$ENV{OPENSSL_PATH_LIBS}]")
else()
	message( STATUS "using openssl headers [$ENV{OPENSSL_PATH_HEADERS}] libs [$ENV{OPENSSL_PATH_LIBS}]")
endif()
include_directories($ENV{OPENSSL_PATH_HEADERS})

#----
#XERCESC
#----
if( NOT DEFINED ENV{XERCESC_PATH_HEADERS} OR NOT DEFINED ENV{XERCESC_PATH_LIBS} )
	message( FATAL_ERROR "unable to determine xerces-c headers and library paths from environment variables XERCESC_PATH_HEADERS [$ENV{XERCESC_PATH_HEADERS}] XERCESC_PATH_LIBS [$ENV{XERCESC_PATH_LIBS}]")
else()
	message( STATUS "using xerces-c headers and library paths from environment variables XERCESC_PATH_HEADERS [$ENV{XERCESC_PATH_HEADERS}] XERCESC_PATH_LIBS [$ENV{XERCESC_PATH_LIBS}]")
endif()
include_directories($ENV{XERCESC_PATH_HEADERS})

#----
#LIBXML2
#----
if( NOT DEFINED ENV{LIBXML2_PATH_HEADERS} OR NOT DEFINED ENV{LIBXML2_PATH_LIBS})
	message( FATAL_ERROR "unable to determine libxml2 headers and library paths from environment variables LIBXML2_PATH_HEADERS [$ENV{LIBXML2_PATH_HEADERS}] LIBXML2_PATH_LIBS [$ENV{LIBXML2_PATH_LIBS}]")
else()
	message( STATUS " using libxml2 headers and library paths from environment variables LIBXML2_PATH_HEADERS [$ENV{LIBXML2_PATH_HEADERS}] LIBXML2_PATH_LIBS [$ENV{LIBXML2_PATH_LIBS}]")
endif()
include_directories($ENV{LIBXML2_HEADERS})

#----
#OPENSSL
#----
if(NOT TARGET libopenssl)
	add_library(libopenssl STATIC IMPORTED)
	set_property(TARGET libopenssl PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/openssl.lib)
endif()
if(NOT TARGET libssl)
	add_library(libssl STATIC IMPORTED)
	set_property(TARGET libssl PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libssl.lib)
endif()
if(NOT TARGET libcrypto)
	add_library(libcrypto STATIC IMPORTED)
	set_property(TARGET libcrypto PROPERTY IMPORTED_LOCATION $ENV{OPENSSL_PATH_LIBS}/libcrypto.lib)
endif()

SET( OPENSSL_LIBS_ALL libopenssl libssl libcrypto )

#-----
#XML Libs
#-----
if(NOT TARGET libxercesc)
	add_library(libxercesc STATIC IMPORTED)
	set_property(TARGET libxercesc PROPERTY IMPORTED_LOCATION $ENV{XERCESC_PATH_LIBS}/xerces-c_3.lib)
endif()
if(NOT TARGET libxml2)
	add_library(libxml2 STATIC IMPORTED)
	set_property(TARGET libxml2 PROPERTY IMPORTED_LOCATION $ENV{LIBXML2_PATH_LIBS}/libxml2.lib)
endif()

SET( XML_LIBS Rpcrt4 crypt32 ws2_32 libxercesc libxml2 ${OPENSSL_LIBS_ALL} )

#-----
#GoogleTest
#-----
include_directories( ${PROJECT_SOURCE_DIR}/GoogleTest/gtest/src/gtest/include )

#------
#OPCUA
#------
add_definitions( -DBACKEND_OPEN62541 )
SET( OPCUA_TOOLKIT_PATH "" )

include_directories (${PROJECT_SOURCE_DIR}/open62541-compat/open62541)

if(NOT TARGET open62541release)
	add_library(open62541release STATIC IMPORTED)
	set_property(TARGET open62541release PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/open62541-compat/open62541/Release/open62541.lib)
endif()

if(NOT TARGET open62541debug)
	add_library(open62541debug STATIC IMPORTED)
	set_property(TARGET open62541debug PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/open62541-compat/open62541/Debug/open62541.lib)
endif()

SET( OPCUA_TOOLKIT_LIBS_RELEASE open62541release )
SET( OPCUA_TOOLKIT_LIBS_DEBUG open62541debug )

#------
#General
#------
add_definitions(-DSUPPORT_XML_CONFIG -DWIN32_LEAN_AND_MEAN)

set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin/)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin/)

SET (CMAKE_EXE_LINKER_FLAGS -v)
