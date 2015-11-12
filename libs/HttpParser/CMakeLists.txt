#
#  Pawel Kubik
#

project(HttpParser)

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_library(${PROJECT_NAME} SHARED ${SRCS})
