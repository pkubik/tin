#
#  Krystian Czapiga
#

project(WebService)

message(STATUS "")
message(STATUS "Configuring ${PROJECT_NAME}...")

# Specify source files
file(GLOB_RECURSE SRCS  *.cpp)

# Set up targets
add_library(${PROJECT_NAME} SHARED ${SRCS})

# Set up dependencies
add_dependencies(${PROJECT_NAME} Common HttpServer DataStore NLTemplate)

target_link_libraries(${PROJECT_NAME} Common HttpServer DataStore NLTemplate)
