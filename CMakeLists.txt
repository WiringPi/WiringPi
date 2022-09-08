cmake_minimum_required(VERSION 3.10)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/get-version.cmake")

project(wiringPi_Main VERSION ${VERSION})

add_subdirectory(wiringPi)
add_subdirectory(devLib)
add_subdirectory(wiringPiD)
add_subdirectory(gpio)
