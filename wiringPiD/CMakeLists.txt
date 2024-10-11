# CMakeLists.txt for wiringPiD
project(wiringPiD)

# Source files
set(SRC wiringpid.c network.c runRemote.c daemonise.c)

# Add the executable
add_executable(wiringpid ${SRC})

# Set C compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -Wextra")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
find_library(WIRINGPI_LIB wiringPi)
find_library(WIRINGPIDEV_LIB wiringPiDev)
find_library(PTHREAD_LIB pthread)
find_library(RT_LIB rt)
find_library(M_LIB m)
find_library(CRYPT_LIB crypt)
target_link_libraries(wiringpid
    ${WIRINGPI_LIB}
    ${WIRINGPIDEV_LIB}
    ${PTHREAD_LIB}
    ${RT_LIB}
    ${M_LIB}
    ${CRYPT_LIB}
)

# Install the executable
install(TARGETS wiringpid
    RUNTIME DESTINATION bin
)
