# CMakeLists.txt for devLib
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

target_include_directories(wiringPiDev PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wiringPiDev PRIVATE wiringPi)

# Set the library version
set_target_properties(wiringPiDev PROPERTIES VERSION ${WIRINGPI_VERSION} SOVERSION ${WIRINGPI_VERSION_MAJOR})

# Install headers
install(FILES ${HEADERS} DESTINATION include COMPONENT Development)
# Install the library
install(TARGETS wiringPiDev LIBRARY DESTINATION lib COMPONENT Runtime NAMELINK_COMPONENT Development)
