cmake_minimum_required(VERSION 3.10)
OPTION( NO_CRYPT "Disable encryption usage" OFF )

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/get-version.cmake")

project(wiringPi VERSION ${VERSION})
set(TARGET wiringPi)

# Have CMake find pthreads library within our toolchain
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

# Source Files
set(wiringPi_src
    ads1115.c bmp180.c drcNet.c drcSerial.c
    ds18b20.c htu21d.c max31855.c max5322.c
    mcp23008.c mcp23016.c mcp23017.c mcp23s08.c
    mcp23s17.c mcp3002.c mcp3004.c mcp3422.c
    mcp4802.c pcf8574.c pcf8591.c piHiPri.c
    piThread.c pseudoPins.c rht03.c sn3218.c
    softPwm.c softServo.c softTone.c sr595.c
    wiringPi.c wiringPiI2C.c wiringPiSPI.c
    wiringSerial.c wiringShift.c wpiExtensions.c
)

# Headers file
set(wiringPi_h
    ads1115.h bmp180.h drcNet.h drcSerial.h
    ds18b20.h htu21d.h max31855.h max5322.h
    mcp23008.h mcp23016.h mcp23016reg.h mcp23017.h
    mcp23s08.h mcp23s17.h mcp23x08.h mcp23x0817.h
    mcp3002.h mcp3004.h mcp3422.h mcp4802.h
    pcf8574.h pcf8591.h pseudoPins.h rht03.h
    sn3218.h softPwm.h softServo.h softTone.h
    sr595.h wiringPi.h wiringPiI2C.h wiringPiSPI.h
    wiringSerial.h wiringShift.h wpiExtensions.h
)

# Add library target
add_library (${TARGET} SHARED ${wiringPi_src})

# Version
set_target_properties(${TARGET} PROPERTIES
                        VERSION ${PROJECT_VERSION}
                        SOVERSION ${PROJECT_VERSION_MAJOR})

# Add include directory for wiringPi
target_include_directories (${TARGET} PUBLIC 
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_INSTALL_PREFIX}/include)


# Add the following required libraries: Threads, Math, Crypt, and RealTime
target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT} $<$<NOT:$<BOOL:${NO_CRYPT}>>:crypt> m rt)
target_compile_definitions(${TARGET} PRIVATE $<$<BOOL:${NO_CRYPT}>:NO_CRYPT>)

# Expose wiringPi public to other subprojects
set(${TARGET}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include directories" FORCE
)

# Add install
install(TARGETS ${TARGET} DESTINATION lib)
install(FILES ${wiringPi_h} DESTINATION include)
