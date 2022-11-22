# @author Damian Abalo
# @date 03-Sep-2015
# The purpose of this file is to set default parameters in case no build configuration file (aka toolchain) was specified.

# The approach is to satisfy the requirements as much as possible.

message("using build configuration from enice_win_configuration.cmake")
message(STATUS "environment vars: BOOST_HOME [$ENV{BOOST_HOME}] UNIFIED_AUTOMATION_HOME [$ENV{UNIFIED_AUTOMATION_HOME}] CODE_SYNTHESYS_XSD [$ENV{CODE_SYNTHESYS_XSD}] OPENSSL [$ENV{OPENSSL}] XERCESC [$ENV{XERCESC}] LIBXML2[$ENV{LIBXML2}]")


#-------
#Boost
#-------
if( NOT DEFINED ENV{BOOST_HOME} )
	message(FATAL_ERROR "environment variable BOOST_HOME not defined - please set this to a 64bit boost installation")
else()	
	SET( BOOST_PATH_HEADERS $ENV{BOOST_HOME}/include/boost-1_59 )
	SET( BOOST_PATH_LIBS $ENV{BOOST_HOME}/lib	 )
endif()
message(STATUS "BOOST - include [${BOOST_PATH_HEADERS}] libs [${BOOST_PATH_LIBS}]")

if(NOT TARGET libboostprogramoptions)
	add_library(libboostprogramoptions STATIC IMPORTED)
	set_property(TARGET libboostprogramoptions PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_program_options-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostsystem)
	add_library(libboostsystem STATIC IMPORTED)
	set_property(TARGET libboostsystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_system-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostfilesystem)
	add_library(libboostfilesystem STATIC IMPORTED)
	set_property(TARGET libboostfilesystem PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_filesystem-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostchrono) 
	add_library(libboostchrono STATIC IMPORTED)
	set_property(TARGET libboostchrono PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_chrono-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostdatetime) 
	add_library(libboostdatetime STATIC IMPORTED)
	set_property(TARGET libboostdatetime PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_date_time-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostthread) 
	add_library(libboostthread STATIC IMPORTED)
	set_property(TARGET libboostthread PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_thread-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostlog)
	add_library(libboostlog STATIC IMPORTED)
	set_property(TARGET libboostlog PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_log-vc120-mt-1_59.lib)
endif()
if(NOT TARGET libboostlogsetup)
	add_library(libboostlogsetup STATIC IMPORTED)
	set_property(TARGET libboostlogsetup PROPERTY IMPORTED_LOCATION ${BOOST_PATH_LIBS}/libboost_log_setup-vc120-mt-1_59.lib)
endif()

#set( BOOST_LIBS libboost_system-vc120-mt-1_57 libboost_program_options-vc120-mt-1_57 libboost_thread-vc120-mt-1_57 libboost_date_time-vc120-mt-1_57 libboost_chrono-vc120-mt-1_57 libboost_regex-vc120-mt-1_57 -lrt)
set( BOOST_LIBS  libboostlogsetup libboostlog libboostsystem libboostfilesystem libboostthread libboostprogramoptions libboostchrono libboostdatetime -lrt)

#------
#OPCUA
#------
if( NOT DEFINED ENV{UNIFIED_AUTOMATION_HOME} )
	message(FATAL_ERROR "environment variable UNIFIED_AUTOMATION_HOME not defined - please set this to a 64bit unified automation toolkit installation")
else()
	SET( OPCUA_TOOLKIT_PATH $ENV{UNIFIED_AUTOMATION_HOME} )		
endif()
message(STATUS "UA TOOLKIT - OPC-UA toolkit path [${OPCUA_TOOLKIT_PATH}]" )

if(NOT TARGET libuamodule)
	add_library(libuamodule STATIC IMPORTED)
	set_property(TARGET libuamodule PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uamodule.lib)
endif()
if(NOT TARGET libcoremodule)
	add_library(libcoremodule STATIC IMPORTED)
	set_property(TARGET libcoremodule PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/coremodule.lib)
endif()
if(NOT TARGET libuabase)
	add_library(libuabase STATIC IMPORTED)
	set_property(TARGET libuabase PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uabase.lib)
endif()
if(NOT TARGET libuastack)
	add_library(libuastack STATIC IMPORTED)
	set_property(TARGET libuastack PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uastack.lib)
endif()
if(NOT TARGET libuapki)
	add_library(libuapki STATIC IMPORTED)
	set_property(TARGET libuapki PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uapki.lib)
endif()
if(NOT TARGET libxmlparser)
	add_library(libxmlparser STATIC IMPORTED)
	set_property(TARGET libxmlparser PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/xmlparser.lib)
endif()

if(NOT TARGET libuamoduled)
	add_library(libuamoduled STATIC IMPORTED)
	set_property(TARGET libuamoduled PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uamoduled.lib)
endif()
if(NOT TARGET libcoremoduled)
	add_library(libcoremoduled STATIC IMPORTED)
	set_property(TARGET libcoremoduled PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/coremoduled.lib)
endif()
if(NOT TARGET libuabased)
	add_library(libuabased STATIC IMPORTED)
	set_property(TARGET libuabased PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uabased.lib)
endif()
if(NOT TARGET libuastackd)
	add_library(libuastackd STATIC IMPORTED)
	set_property(TARGET libuastackd PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uastackd.lib)
endif()
if(NOT TARGET libuapkid)
	add_library(libuapkid STATIC IMPORTED)
	set_property(TARGET libuapkid PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/uapkid.lib)
endif()
if(NOT TARGET libxmlparserd)
	add_library(libxmlparserd STATIC IMPORTED)
	set_property(TARGET libxmlparserd PROPERTY IMPORTED_LOCATION ${OPCUA_TOOLKIT_PATH}/lib/xmlparserd.lib)
endif()

SET( OPCUA_TOOLKIT_LIBS_DEBUG libuamoduled libcoremoduled libuabased libuastackd libuapkid libxmlparserd ) 
SET( OPCUA_TOOLKIT_LIBS_RELEASE libuamodule libcoremodule libuabase libuastack libuapki libxmlparser )

#-----
# LogIt
#-----
SET( LOGIT_HAS_STDOUTLOG TRUE )
SET( LOGIT_HAS_BOOSTLOG TRUE )
SET( LOGIT_HAS_UATRACE FALSE )
MESSAGE( STATUS "LogIt build options: stdout [${LOGIT_HAS_STDOUTLOG}] boost [${LOGIT_HAS_BOOSTLOG}] uaTrace [${LOGIT_HAS_UATRACE}]" )

#-----
#CodeSynthesys XSD
#-----
if( NOT DEFINED ENV{CODE_SYNTHESYS_XSD} )
	message(WARNING "environment variable CODE_SYNTHESYS_XSD not defined - using default location [C:/Program Files (x86)/CodeSynthesis XSD 3.3]")
	include_directories(
	"C:/Program Files (x86)/CodeSynthesis XSD 3.3/include"
	)
else()
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

#-----
#XML Libs
#-----
if(NOT TARGET libxercesc) 
	add_library(libxercesc STATIC IMPORTED)
	set_property(TARGET libxercesc PROPERTY IMPORTED_LOCATION ${XERCESC_PATH}/VC10/Debug/xerces-c_3D.lib)		
endif()
if(NOT TARGET custlibxml) 
	add_library(custlibxml STATIC IMPORTED)
	set_property(TARGET custlibxml PROPERTY IMPORTED_LOCATION ${LIBXML2_PATH}/lib/libxml2.lib)	
endif()
if(NOT TARGET custeay32) 
	add_library(custeay32 STATIC IMPORTED)
	set_property(TARGET custeay32 PROPERTY IMPORTED_LOCATION ${OPENSSL_PATH}/lib/libeay32.lib)
endif()

SET( XML_LIBS Rpcrt4 crypt32 ws2_32 custeay32 libxercesc custlibxml)

add_definitions(-DSUPPORT_XML_CONFIG -Wall -DWIN32_LEAN_AND_MEAN)
set(CMAKE_CXX_FLAGS_RELEASE "/MD")
set(CMAKE_CXX_FLAGS_DEBUG "/MDd /Zi")

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin/)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin/)