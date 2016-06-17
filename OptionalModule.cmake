cmake_minimum_required(VERSION 2.8.12)

message ("Configuring optional module ${OPT_MODULE_NAME}")
project(${OPT_MODULE_NAME}_download NONE)
include(ExternalProject)
ExternalProject_Add(
    ${OPT_MODULE_NAME}
    GIT_REPOSITORY ${OPT_MODULE_URL}
    GIT_TAG ${OPT_MODULE_TAG}
    PREFIX "${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME}_download"
    SOURCE_DIR "${CMAKE_BINARY_DIR}/${OPT_MODULE_NAME}"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ""
)
message ("Configure of optional module ${OPT_MODULE_NAME} finished")
