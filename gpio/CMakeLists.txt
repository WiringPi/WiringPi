# CMakeLists.txt for gpio
project(gpio)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libcrypt REQUIRED IMPORTED_TARGET)

find_package(Threads REQUIRED)

# Source files
set(SRC
    gpio.c
    readall.c
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Add the executable target
add_executable(gpio ${SRC})

# Link the required libraries
target_link_libraries(gpio wiringPi wiringPiDev Threads::Threads m PkgConfig::libcrypt)

# Install the executable
install(TARGETS gpio DESTINATION bin COMPONENT Runtime)

# Install man page
install(FILES gpio.1 DESTINATION share/man/man1 COMPONENT Runtime)
