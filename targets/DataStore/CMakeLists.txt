#
#  Pawel Kubik
#

project(DataStore)

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_library(${PROJECT_NAME} SHARED ${SRCS})

# Set up dependencies
find_package(Qt5 COMPONENTS Sql REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Sql)
