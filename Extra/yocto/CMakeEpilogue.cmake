INSTALL(TARGETS ${EXECUTABLE} DESTINATION ${CMAKE_INSTALL_PREFIX} )
INSTALL(FILES
	${PROJECT_SOURCE_DIR}/bin/config.xml
	${PROJECT_BINARY_DIR}/Configuration/Configuration.xsd
	DESTINATION
	${CMAKE_INSTALL_PREFIX}
	)