set(devLib_src ds1302.c maxdetect.c scrollPhat.c piNes.c gertboard.c piFace.c lcd128x64.c lcd.c piGlow.c)
set(LIB_INSTALL_PATH /usr/lib)
add_library(libwiringPiDev  SHARED ${devLib_src})
set(LIBRARY_OUTPUT_PATH {LIB_INSTALL_PATH)
add_definitions("-lpthread")
set_target_properties(libwiringPiDev PROPERTIES OUTPUT_NAME "wiringPiDev")
install(TARGETS libwiringPiDev DESTINATION ${LIB_INSTALL_PATH})


