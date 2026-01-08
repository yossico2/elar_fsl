cmake_minimum_required(VERSION 3.10)

# Define project name once
set(PROJECT_NAME fsl)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif()

if("${TARGET}" STREQUAL "linux")
    set(ELAR_ARCH "x86_64")
elseif("${TARGET}" STREQUAL "arm")
    set(ELAR_ARCH "arm")
else()
    message(FATAL_ERROR "Unknown TARGET: ${TARGET}. Use 'linux' or 'arm'.")
endif()

# Set default TARGET to linux if not defined (use CACHE to ensure global visibility)
if(NOT DEFINED TARGET OR "${TARGET}" STREQUAL "" OR "${TARGET}" STREQUAL ".")
    set(TARGET "linux" CACHE STRING "Target platform (linux or arm)")
endif()

set(APP_SOURCES
    src/main.cpp 
    src/app.cpp 
    src/config.cpp
    src/sdk/tinyxml2/tinyxml2.cpp
    src/sdk/logger.cpp
    src/sdk/udp.cpp
    src/sdk/uds.cpp
)

set(TESTS_SOURCES
    tests/main.cpp
    tests/test_config.cpp
    tests/test_ctrl_status.cpp
    src/app.cpp
    src/config.cpp
    src/sdk/tinyxml2/tinyxml2.cpp
    src/sdk/logger.cpp
    src/sdk/udp.cpp
    src/sdk/uds.cpp
)

add_executable(tests ${TESTS_SOURCES})
target_include_directories(tests PRIVATE tests src src/sdk src/sdk/nlohmann src/sdk/tinyxml2)

# set(LIBSDK_PATH "$ENV{HOME}/dev/elar/sdk/lib/${ELAR_ARCH}/libsdk.a")
# target_link_libraries(tests ${LIBSDK_PATH} pthread)
target_link_libraries(tests pthread)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

# Set default build type to debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug CACHE STRING "Build type" FORCE)
endif()

# Platform selection
if(DEFINED ENV{PLATFORM_ARM})
    message(STATUS "Compiling for Arm")
    # Example: set cross-compiler and flags for Arm
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER $ENV{PETALINUX_C_COMPILER})
    set(CMAKE_CXX_COMPILER $ENV{PETALINUX_CXX_COMPILER})
    add_definitions(-DPLATFORM_ARM)
else()
    message(STATUS "Compiling for Ubuntu/Linux")
endif()

add_executable(${PROJECT_NAME} ${APP_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE src src/sdk src/sdk/nlohmann src/sdk/tinyxml2)
target_link_libraries(${PROJECT_NAME} pthread)

# Add include directories
include_directories(src)

# Copy config.xml to the build output directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/src/config.xml
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.xml)
