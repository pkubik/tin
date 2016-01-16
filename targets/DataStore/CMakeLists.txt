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

#SET(GCC_COVERAGE_LINK_FLAGS    "-lpq -lpqxx")
#add_definitions(${GCC_COVERAGE_LINK_FLAGS})
target_link_libraries(${PROJECT_NAME} "-lpq -lpqxx")


# Set up dependencies
find_package(Qt5 COMPONENTS Sql REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Sql)
