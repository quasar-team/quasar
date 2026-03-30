
# check quasar/Common/CommonCustom.cmake for adding custom sources!
if(EXISTS ${PROJECT_SOURCE_DIR}/Common/CommonCustom.cmake)
  include (CommonCustom.cmake)
endif(EXISTS ${PROJECT_SOURCE_DIR}/Common/CommonCustom.cmake)

add_library (Common OBJECT
	src/ASUtils.cpp
        src/QuasarThreadPool.cpp
	${COMMON_CUSTOM_SOURCES}
	)

if (BUILD_QUASAR_TESTS)        
link_directories(
        ${OPCUA_TOOLKIT_PATH}/lib
        ${BOOST_PATH_LIBS}
        ${SERVER_LINK_DIRECTORIES}
)       

add_executable(test_quasar_threadpool
        test/test_quasar_threadpool.cpp
        $<TARGET_OBJECTS:Common>
        $<TARGET_OBJECTS:LogIt>
        
        )
 
        
target_link_libraries( test_quasar_threadpool
        ${OPCUA_TOOLKIT_LIBS_DEBUG}
)
endif(BUILD_QUASAR_TESTS)