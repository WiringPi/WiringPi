set(wiringPi_src wiringPi.c						
		wiringSerial.c wiringShift.c				
		piHiPri.c piThread.c					
		wiringPiSPI.c wiringPiI2C.c				
		softPwm.c softTone.c					
		mcp23008.c mcp23016.c mcp23017.c			
		mcp23s08.c mcp23s17.c					
		sr595.c							
		pcf8574.c pcf8591.c					
		mcp3002.c mcp3004.c mcp4802.c mcp3422.c
		max31855.c max5322.c	
		sn3218.c	
                drcSerial.c						
		wpiExtensions.c)
set(LIB_INSTALL_PATH /usr/lib)
set(LIB_INSTALL_HEADERS_PATH /usr/local)
add_library(libwiringPi  SHARED ${wiringPi_src})
set(LIBRARY_OUTPUT_PATH {LIB_INSTALL_PATH)
add_definitions("-lpthread")
set_target_properties(libwiringPi PROPERTIES OUTPUT_NAME "wiringPi")

set(root_HEADERS 
                wiringPi.h						
		wiringSerial.h wiringShift.h				
		wiringPiSPI.h wiringPiI2C.h				
		softPwm.h softTone.h					
		mcp23008.h mcp23016.h mcp23017.h			
		mcp23s08.h mcp23s17.h					
		sr595.h							
		pcf8574.h pcf8591.h					
		mcp3002.h mcp3004.h mcp4802.h mcp3422.h			
		max31855.h max5322.h					
		sn3218.h						
		drcSerial.h						
		wpiExtensions.h 
)

install(FILES ${root_HEADERS} DESTINATION ${LIB_INSTALL_HEADERS_PATH}/include)
install(TARGETS libwiringPi DESTINATION ${LIB_INSTALL_PATH})
