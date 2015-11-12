#
#  TIN 2015
#
#  Krystian Czapiga
#  Robert Kluz
#  Pawel Kubik
#  Patryk Szypulski
#

# Set up main project
#
cmake_minimum_required(VERSION 2.8)

project(HttpServer)
SET(PROJECT_AUTHOR "Pawel Kubik")

# Path for additional modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify options
#
option(BUILD_TESTS "build tests" ON)

# Set up general include directories
#
include_directories(SYSTEM ./libs)

# Set up DEBUG defines
#
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG DEBUG=1)

# Specify build paths
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${HttpServer_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${HttpServer_DIR}")

# CXX Compiler settings
#
if (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++14 -pedantic")
elseif (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE")
endif (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)

# Build components
#

# specify subprojects
add_subdirectory(libs/HttpParser)
#add_subdirectory(libs/AnyNewLibrary)

# build main executable
add_subdirectory(HttpServer)

# optionally build tests
if (BUILD_TESTS)
    # add tests subdirectory
endif()
