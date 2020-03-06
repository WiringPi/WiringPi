cmake_minimum_required(VERSION 3.0)

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

# Have CMake find our pthreads library within our toolchain (required for this library)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# add all the *.c files as sources
FILE(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/wiringPiD/*.c
                    ${PROJECT_SOURCE_DIR}/wiringPi/*.c
)

# add all the *.h files as headers
FILE(GLOB HEADERS ${PROJECT_SOURCE_DIR}/devLib/*.h
                  ${PROJECT_SOURCE_DIR}/wiringPi/*.h
)

# make this output a shared library (with .so output)
add_library (wiringPi SHARED ${SRC_FILES})

# be sure to include the current source directory for header files
target_include_directories (wiringPi PUBLIC
	${PROJECT_SOURCE_DIR}/wiringPi
	${PROJECT_SOURCE_DIR}/devLib
)

# add the following required libraries:
# Threads, Math, Crypt, and RealTime
target_link_libraries(wiringPi ${CMAKE_THREAD_LIBS_INIT} crypt m rt)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS wiringPi EXPORT rpi
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

