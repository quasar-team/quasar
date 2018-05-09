

add_library (Common OBJECT
	src/ASUtils.cpp
        src/QuasarThreadPool.cpp
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