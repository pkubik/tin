#
#  TIN 2015
#
#  Krystian Czapiga
#  Robert Kluz
#  Pawel Kubik
#  Patryk Szypulski
#

SET(UT_CODENAME "ut${PROJECT_NAME}")

message(STATUS "")
message(STATUS "Configuring ${UT_CODENAME}...")

# Specify source files
file(GLOB_RECURSE SRCS ut/*.cpp)

# Set up targets
add_executable(${UT_CODENAME} ${SRCS})

# Set up include directories
#
include_directories(${TESTS_DIR})

# Set up dependencies
add_dependencies(${UT_CODENAME} Common DataStore Network HttpServer HttpParser)

target_link_libraries(${UT_CODENAME} Common DataStore Network HttpServer HttpParser)
