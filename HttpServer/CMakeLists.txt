#
#  Pawel Kubik
#

SET(SERVER_CODENAME "${PROJECT_NAME}")

message(STATUS "")
message(STATUS "Configuring ${SERVER_CODENAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_executable(${SERVER_CODENAME} ${SRCS})

# Set up dependencies
add_dependencies(${SERVER_CODENAME} HttpParser)

target_link_libraries(${SERVER_CODENAME} HttpParser)
