# @author Damian Abalo
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.

message("using build configuration from enice_win_configuration.cmake")
message(STATUS "environment vars: BOOST_HOME [$ENV{BOOST_HOME}] CODE_SYNTHESYS_XSD [$ENV{CODE_SYNTHESYS_XSD}] OPENSSL [$ENV{OPENSSL}] XERCESC [$ENV{XERCESC}] LIBXML2[$ENV{LIBXML2}]")


#-------
#Boost
#-------
if( NOT DEFINED ENV{BOOST_HOME} )
	message(FATAL_ERROR "environment variable BOOST_HOME not defined - please set this to a 64bit boost installation")
else()	
	SET( BOOST_PATH_HEADERS $ENV{BOOST_HOME}/include )
	SET( BOOST_PATH_LIBS $ENV{BOOST_HOME}/lib	 )
endif()
message(STATUS "BOOST - include [${BOOST_PATH_HEADERS}] libs [${BOOST_PATH_LIBS}]")

if(NOT TARGET libboostprogramoptions)
	add_library(libboostprogramoptions STATIC IMPORTED)
	set_property(TARGET libboostprogramoptions PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_program_options-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostsystem)
	add_library(libboostsystem STATIC IMPORTED)
	set_property(TARGET libboostsystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_system-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostfilesystem)
	add_library(libboostfilesystem STATIC IMPORTED)
	set_property(TARGET libboostfilesystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_filesystem-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostchrono) 
	add_library(libboostchrono STATIC IMPORTED)
	set_property(TARGET libboostchrono PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_chrono-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostdatetime) 
	add_library(libboostdatetime STATIC IMPORTED)
	set_property(TARGET libboostdatetime PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_date_time-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostthread) 
	add_library(libboostthread STATIC IMPORTED)
	set_property(TARGET libboostthread PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_thread-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostlog)
	add_library(libboostlog STATIC IMPORTED)
	set_property(TARGET libboostlog PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_log-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostlogsetup)
	add_library(libboostlogsetup STATIC IMPORTED)
	set_property(TARGET libboostlogsetup PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_1_59_0_log_setup-vc120-mt-1_59.lib)
endif()

#set( BOOST_LIBS libboost_system-vc120-mt-1_57 libboost_program_options-vc120-mt-1_57 libboost_thread-vc120-mt-1_57 libboost_date_time-vc120-mt-1_57 libboost_chrono-vc120-mt-1_57 libboost_regex-vc120-mt-1_57 -lrt)
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
if( NOT DEFINED ENV{CODE_SYNTHESYS_XSD} )
	message(WARNING "environment variable CODE_SYNTHESYS_XSD not defined - using default location [C:/Program Files (x86)/CodeSynthesis XSD 4.0]")
	include_directories(
	"C:/Program Files (x86)/CodeSynthesis XSD 4.0/include"
	)
else()
	message(STATUS "Using environment variable for CODE_SYNTHESYS_XSD adding include [$ENV{CODE_SYNTHESYS_XSD}/include]")
	include_directories(
	$ENV{CODE_SYNTHESYS_XSD}/include
	)
endif()
		

#----
#OPENSSL
#----
if( NOT DEFINED ENV{OPENSSL} )
	message(WARNING "environment variable OPENSSL not defined - using default location [C:/OpenSSL-Win64]")
	SET(OPENSSL_PATH "C:/OpenSSL-Win64")
else()
	SET(OPENSSL_PATH $ENV{OPENSSL})
endif()

include_directories(
	${OPENSSL_PATH}/include
)
message(STATUS "Open SSL path [${OPENSSL_PATH}]" )

#----
#XERCESC
#----
if( NOT DEFINED ENV{XERCESC} )
	message(WARNING "environment variable XERCESC not defined - using default location [C:/3rdPartySoftware/xerces-c-3.1.2/Build/Win64]")
	SET(XERCESC_PATH "C:/3rdPartySoftware/xerces-c-3.1.2/Build/Win64")
else()
	SET(XERCESC_PATH $ENV{XERCESC})
endif()
message(STATUS "Xerces-c path [${XERCESC_PATH}]" )

#----
#LIBXML2
#----
if( NOT DEFINED ENV{LIBXML2} )
	message(WARNING "environment variable LIBXML2 not defined - using default location [C:/3rdPartySoftware/libxml2/v2.7.8_64BIT/BUILD]")
	SET(LIBXML2_PATH "C:/3rdPartySoftware/libxml2/v2.7.8_64BIT/BUILD/VS2013")
else()
	SET(LIBXML2_PATH $ENV{LIBXML2})
endif()
message(STATUS "Lib XML 2 path [${LIBXML2_PATH}]" )

#----
#OPENSSL
#----
if(NOT TARGET custlibopenssl) 
	add_library(custlibopenssl STATIC IMPORTED)
	set_property(TARGET custlibopenssl PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/openssl.lib)
endif()
if(NOT TARGET custlibssl) 
	add_library(custlibssl STATIC IMPORTED)
	set_property(TARGET custlibssl PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/libssl.lib)
endif()
if(NOT TARGET custlibcrypto) 
	add_library(custlibcrypto STATIC IMPORTED)
	set_property(TARGET custlibcrypto PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/libcrypto.lib)
endif()

SET( OPENSSL_LIBS custlibopenssl custlibssl custlibcrypto )

#-----
#XML Libs
#-----
if(NOT TARGET libxercesc) 
	add_library(libxercesc STATIC IMPORTED)
	set_property(TARGET libxercesc PROPERTY IMPORTED_LOCATION ${XERCESC_PATH}/VC10/Debug/xerces-c_3D.lib)		
endif()
if(NOT TARGET custlibxml) 
	add_library(custlibxml STATIC IMPORTED)
	set_property(TARGET custlibxml PROPERTY IMPORTED_LOCATION ${LIBXML2_PATH}/libxml2.lib)	
endif()


SET( XML_LIBS Rpcrt4 crypt32 ws2_32 libxercesc custlibxml ${OPENSSL_LIBS} )

#-----
#GoogleTest
#-----
include_directories(
	${PROJECT_SOURCE_DIR}/GoogleTest/gtest/src/gtest/include
)

#------
#OPCUA
#------
add_definitions( -DBACKEND_OPEN62541 )
SET( CUSTOM_SERVER_MODULES open62541-compat)
SET( OPCUA_TOOLKIT_PATH "" )

SET( OPCUA_TOOLKIT_LIBS_RELEASE "${PROJECT_SOURCE_DIR}/open62541-compat/open62541/build/Release/open62541.lib" )
SET( OPCUA_TOOLKIT_LIBS_DEBUG "${PROJECT_SOURCE_DIR}/open62541-compat/open62541/build/Release/open62541.lib" )

#------
#General
#------
add_definitions(-DSUPPORT_XML_CONFIG -DWIN32_LEAN_AND_MEAN)

set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin/)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin/)

SET (CMAKE_EXE_LINKER_FLAGS -v)
