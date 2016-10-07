cmake_minimum_required(VERSION 2.8)
project(WiringPi_lib)

add_subdirectory(devLib)
add_subdirectory(gpio)
#add_subdirectory(pins)
add_subdirectory(wiringPi)

# Build deb package
# command to build *.deb
# cmake . && make && make package
SET(MAJOR_VERSION 2)
SET(MINOR_VERSION 32)
SET(PATCH_VERSION 0)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_DESCRIPTION "WiringPi Library for RaspberryPi")
SET(CPACK_PACKAGE_CONTACT "sndnvaps@gmail.com")
INCLUDE(CPack)

