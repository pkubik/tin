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
cmake_minimum_required(VERSION 3.0)

project(Tin)
set(PROJECT_AUTHOR "Pawel Kubik")

# Path for additional modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify options
#
option(BUILD_TESTS "build tests" ON)

# Subdirectories
#
set(TARGETS_DIR ${PROJECT_SOURCE_DIR}/targets)
set(TESTS_DIR ${PROJECT_SOURCE_DIR}/tests)

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
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "DEBUG")
endif(NOT CMAKE_BUILD_TYPE)


if (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)
    if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -pthread -pedantic -O0 -ggdb")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall -Wextra -pthread -pedantic -O0 -ggdb")
    else ()
        set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -pthread -pedantic -O2 -DNDEBUG")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -Wall -Wextra -pthread -pedantic -O2 -DNDEBUG")
    endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)
elseif (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE")
endif (CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID STREQUAL Clang)

# Build components
#

# specify subprojects

add_subdirectory(targets/Common)
add_subdirectory(targets/DataStore)
add_subdirectory(targets/Network)
add_subdirectory(targets/HttpParser)
add_subdirectory(targets/HttpServer)
add_subdirectory(targets/WebService)
add_subdirectory(targets/NLTemplate)
#add_subdirectory(targets/AnyNewTarget)

# build main executable
add_subdirectory(targets/Application)

# optionally build tests
if (BUILD_TESTS)
    add_subdirectory(${TESTS_DIR})
endif()

# copy resources
#
file(COPY res DESTINATION .)
