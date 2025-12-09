cmake_minimum_required(VERSION 3.10)
project(fsl)

set(CMAKE_CXX_STANDARD 17)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Platform selection
if(DEFINED ENV{PETALINUX})
    message(STATUS "Compiling for PetaLinux")
    # Example: set cross-compiler and flags for PetaLinux
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_C_COMPILER $ENV{PETALINUX_C_COMPILER})
    set(CMAKE_CXX_COMPILER $ENV{PETALINUX_CXX_COMPILER})
    add_definitions(-DPETALINUX)
else()
    message(STATUS "Compiling for Ubuntu/Linux")
endif()

add_executable(fsl src/main.cpp src/example.cpp)

# Add include directories
include_directories(src)
