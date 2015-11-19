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

project(Tin)
SET(PROJECT_AUTHOR "Pawel Kubik")

# Path for additional modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify options
#
option(BUILD_TESTS "build tests" ON)

# Subdirectories
#
SET(TARGETS_DIR ${PROJECT_SOURCE_DIR}/targets)
SET(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

# Set up general include directories
#
include_directories(SYSTEM ${TARGETS_DIR})

# Set up DEBUG defines
#
set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG DEBUG=1)

# Specify build paths
#
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# CXX Compiler settings
#
if (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -pedantic")
elseif (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE")
endif (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)

# Build components
#

# specify subprojects

add_subdirectory(targets/HttpParser)
add_subdirectory(targets/HttpServer)
#add_subdirectory(targets/AnyNewTarget)

# build main executable
add_subdirectory(targets/Application)

# optionally build tests
if (BUILD_TESTS)
    add_subdirectory(${TESTS_DIR})
endif()
