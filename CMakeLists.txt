cmake_minimum_required(VERSION 3.10)
project(WiringPiLib)

# Read version from version.h
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.h" WIRINGPI_VERSION)
string(REGEX REPLACE ".*VERSION \"([0-9]+.[0-9]+)\".*" "\\1" WIRINGPI_VERSION "${WIRINGPI_VERSION}")
string(REGEX REPLACE "([0-9]+)\..*" "\\1" WIRINGPI_SOVERSION "${WIRINGPI_VERSION}")

# Include subdirectories
add_subdirectory(devLib)
add_subdirectory(gpio)
#add_subdirectory(pins)
add_subdirectory(wiringPi)
#add_subdirectory(wiringPiD)
