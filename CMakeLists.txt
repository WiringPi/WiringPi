cmake_minimum_required(VERSION 3.10)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/get-version.cmake")

project(wiringPi_lib VERSION ${VERSION})

add_subdirectory(wiringPi)
add_subdirectory(devLib)
add_subdirectory(wiringPiD)
add_subdirectory(gpio)

# CPack
set(CPACK_PACKAGE_NAME "wiringPi")
set(CPACK_PACKAGE_DESCRIPTION "WiringPi Library for RaspberryPi")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/WiringPi/WiringPi")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "WiringPi")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

include(CPack)