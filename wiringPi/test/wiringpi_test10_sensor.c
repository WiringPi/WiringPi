#include "wpi_test.h"
#include <ds18b20.h> 
#include <unistd.h>


#define GROUND_T_PIN_BASE 100

int main (void) {

  printf("Load w1-gpio overlay at GPIO4\n");
  system("sudo dtoverlay w1-gpio gpiopin=4 pullup=1");
  sleep(2);

  ds18b20Setup(GROUND_T_PIN_BASE, "011879ee7fff");

  int value = analogRead(GROUND_T_PIN_BASE);
  float fvalue = value/10.0f;
  //printf("Temp.: %4.1f*C (%d)\n", fvalue, value);
  CheckSameFloat("Temperature 18B20 Sensor at GPIO4", fvalue, 24.0f, 4.0f);
  

  return UnitTestState();
}
