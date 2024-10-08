cmake_minimum_required(VERSION 3.12)
project(WiringPiLib)

# Read version from version.h
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.h" WIRINGPI_VERSION)
string(REGEX REPLACE ".*VERSION \"([0-9]+.[0-9]+)\".*" "\\1" WIRINGPI_VERSION "${WIRINGPI_VERSION}")

if(NOT WIRINGPI_VERSION MATCHES "([0-9]+)\.([0-9]+)")
    message(FATAL_ERROR "Could not retrieve WiringPi versions")
endif()

set(WIRINGPI_VERSION_MAJOR ${CMAKE_MATCH_1})
set(WIRINGPI_VERSION_MINOR ${CMAKE_MATCH_2})

# Include subdirectories
add_subdirectory(devLib)
add_subdirectory(gpio)
#add_subdirectory(pins)
add_subdirectory(wiringPi)
#add_subdirectory(wiringPiD)

# CPack
set(CPACK_PACKAGE_NAME "wiringpi")
set(CPACK_PACKAGE_DESCRIPTION
"The wiringPi libraries, headers and gpio command
Libraries to allow GPIO access on a Raspberry Pi from C and C++
programs as well as from the command-line")

set(CPACK_DEBIAN_PACKAGE_SECTION "libraries")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/WiringPi/WiringPi")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.LESSER")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_VERSION_MAJOR ${WIRINGPI_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${WIRINGPI_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH "")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Grazer Computer Club - GC2 <wiringpi@gc2.at>")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS "libc6, libcrypt1")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_CONTROL_EXTRA
    "${CMAKE_CURRENT_SOURCE_DIR}/debian-template/wiringPi/DEBIAN/postinst"
    "${CMAKE_CURRENT_SOURCE_DIR}/debian-template/wiringPi/DEBIAN/postrm")

set(CPACK_DEB_COMPONENT_INSTALL TRUE)
set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_NAME "${CPACK_PACKAGE_NAME}-dev")
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS "${CPACK_DEBIAN_RUNTIME_PACKAGE_NAME}")

# Check for arm or arm64 architecture
# Let CMake determine a value otherwise
if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "armhf")
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "arm64")
endif()

set(CPACK_SOURCE_GENERATOR "TGZ")

include(CPack)
