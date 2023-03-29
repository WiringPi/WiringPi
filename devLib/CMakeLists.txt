cmake_minimum_required(VERSION 3.10)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/get-version.cmake")

project(wiringPiDev VERSION ${VERSION})
set(TARGET wiringPiDev)

# Have CMake find pthreads library within our toolchain
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# Source Files
set(wiringPiDev_src
    ds1302.c maxdetect.c piNes.c gertboard.c
    piFace.c lcd128x64.c lcd.c scrollPhat.c
    piGlow.c
)

# Headers file
set(wiringPiDev_h
    ds1302.h gertboard.h lcd128x64.h lcd.h maxdetect.h
    piFace.h piGlow.h piNes.h scrollPhat.h
)

# Add library target
add_library (${TARGET} SHARED ${wiringPiDev_src})

# Version
set_target_properties(${TARGET} PROPERTIES
                        VERSION ${PROJECT_VERSION}
                        SOVERSION ${PROJECT_VERSION_MAJOR})

# Add include directory for wiringPiDev
target_include_directories (${TARGET} PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/../wiringPi
                            ${CMAKE_INSTALL_PREFIX}/include)

# Add the following required libraries: Threads
target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})

# Add install
install(TARGETS ${TARGET} DESTINATION lib)
install(FILES ${wiringPiDev_h} DESTINATION include)
