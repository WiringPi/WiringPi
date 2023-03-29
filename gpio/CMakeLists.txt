cmake_minimum_required(VERSION 3.10)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/get-version.cmake")

project(gpio VERSION ${VERSION})
set(TARGET gpio)

# Have CMake find pthreads library within our toolchain
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# Source Files
set(gpio_src
    gpio.c readall.c
)

# Add library target
add_executable (${TARGET} ${gpio_src})

# Add include directory for gpio
target_include_directories (${TARGET} PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/../wiringPiDev
                            ${CMAKE_CURRENT_SOURCE_DIR}/../wiringPi
                            ${CMAKE_INSTALL_PREFIX}/include)


# Add the following required libraries: wiringPi, wiringPiDev, Threads, Math, Crypt, and RealTime
target_link_directories(${TARGET} PUBLIC ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(${TARGET} wiringPi wiringPiDev ${CMAKE_THREAD_LIBS_INIT} crypt m rt)

# Add install
install(TARGETS ${TARGET} DESTINATION bin)
