#
#  Pawel Kubik
#

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_executable(${PROJECT_NAME} ${SRCS})

# Set up dependencies
add_dependencies(${PROJECT_NAME} HttpServer WebService)

target_link_libraries(${PROJECT_NAME} HttpServer WebService)
