#
#  Pawel Kubik
#

SET(COMMON_CODENAME "Common")

message(STATUS "")
message(STATUS "Configuring ${COMMON_CODENAME}...")

# Specify source files
file(GLOB_RECURSE SRCS *.cpp)

# Set up targets
add_library(${COMMON_CODENAME} STATIC ${SRCS})
