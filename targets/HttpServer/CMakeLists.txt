#
#  Pawel Kubik
#

project(HttpServer)

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_library(${PROJECT_NAME} SHARED ${SRCS})

# Set up dependencies
add_dependencies(${PROJECT_NAME} HttpParser)

target_link_libraries(${PROJECT_NAME} HttpParser)
