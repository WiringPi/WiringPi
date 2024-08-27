# CMakeLists.txt for devLib

cmake_minimum_required(VERSION 3.10)
project(devLib)

# Source files
set(SRC
    ds1302.c maxdetect.c piNes.c
    gertboard.c piFace.c
    lcd128x64.c lcd.c
    scrollPhat.c
    piGlow.c
)

# Headers
set(HEADERS
    ds1302.h maxdetect.h piNes.h
    gertboard.h piFace.h
    lcd128x64.h lcd.h
    scrollPhatFont.h scrollPhat.h
    piGlow.h
)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wformat=2 -Winline -pipe")

# Add the library target
add_library(wiringPiDev SHARED ${SRC})

# Set the library version
set_target_properties(wiringPiDev PROPERTIES VERSION ${VERSION} SOVERSION ${WIRINGPI_SONAME_SUFFIX})

# Install headers
install(FILES ${HEADERS} DESTINATION include)
# Install the library
install(TARGETS wiringPiDev LIBRARY DESTINATION lib)
