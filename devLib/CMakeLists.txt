set(devLib_src ds1302.c maxdetect.c scrollPhat.c piNes.c gertboard.c piFace.c lcd128x64.c lcd.c piGlow.c)
set(LIB_INSTALL_PATH /usr/lib)
set(LIB_INSTALL_HEADERS_PATH /usr/local)
add_library(libwiringPiDev  SHARED ${devLib_src})
set(LIBRARY_OUTPUT_PATH {LIB_INSTALL_PATH)
add_definitions("-lpthread")
set_target_properties(libwiringPiDev PROPERTIES OUTPUT_NAME "wiringPiDev")
set(root_HEADERS 
    ds1302.h gertboard.h lcd.h 
    lcd128x64.h maxdetect.h piFace.h
    piGlow.h piNes.h scrollPhat.h scrollPhatFont.h)
install(FILES ${root_HEADERS} DESTINATION ${LIB_INSTALL_HEADERS_PATH}/include)
install(TARGETS libwiringPiDev DESTINATION ${LIB_INSTALL_PATH})


