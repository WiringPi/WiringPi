# CMakeLists.txt for gpio

cmake_minimum_required(VERSION 3.10)
project(gpio)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libcrypt REQUIRED IMPORTED_TARGET)

# Source files
set(SRC
    gpio.c
    readall.c
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Add the executable target
add_executable(gpio ${SRC})

# Find the required libraries
find_library(WIRINGPI_LIB wiringPi)
find_library(WIRINGPI_DEV_LIB wiringPiDev)

# Link the required libraries
target_link_libraries(gpio ${WIRINGPI_LIB} ${WIRINGPI_DEV_LIB} pthread rt m PkgConfig::libcrypt)

# Install the executable
install(TARGETS gpio DESTINATION bin)

# Install man page
install(FILES gpio.1 DESTINATION share/man/man1)
