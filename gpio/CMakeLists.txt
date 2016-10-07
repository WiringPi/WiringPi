include_directories(${PROJECT_SOURCE_DIR}/devLib)
include_directories(${PROJECT_SOURCE_DIR}/wiringPi)
#project(gpio)
set(GPIO_SRC gpio.c readall.c pins.c)
set(GPIO_BINARY_PATH /usr/bin)
add_executable(gpio ${GPIO_SRC})
target_link_libraries(gpio libwiringPi libwiringPiDev -lpthread -lm)
set(EXECUTABLE_OUTPUT_PATH ${GPIO_BINARY_PATH})
install(TARGETS gpio 
             RUNTIME DESTINATION bin)

