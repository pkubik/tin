# The MIT License (MIT)
# Copyright (c) 2013 Tomas Andrle
#  

project(NLTemplate)

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_library(${PROJECT_NAME} SHARED ${SRCS})