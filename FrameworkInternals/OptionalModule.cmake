cmake_minimum_required(VERSION 2.8)

#message ("Configuring optional module ${OPT_MODULE_NAME}")
project(${OPT_MODULE_NAME}_download NONE)
include(ExternalProject)
ExternalProject_Add(
    ${OPT_MODULE_NAME}
    GIT_REPOSITORY ${OPT_MODULE_URL}
    GIT_TAG ${OPT_MODULE_TAG}
    PREFIX "${CMAKE_BINARY_DIR}/FrameworkInternals/EnabledModules/${OPT_MODULE_NAME}_download"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
    LOG_UPDATE 1              # Wrap update in script to log output
    LOG_CONFIGURE 1           # Wrap configure in script to log output
    LOG_BUILD 1               # Wrap build in script to log output
    LOG_TEST 1                # Wrap test in script to log output
    LOG_INSTALL 1 
)
#message ("Configure of optional module ${OPT_MODULE_NAME} finished")
