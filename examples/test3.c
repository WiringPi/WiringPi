
/*
 * test2.c:
 *	Simple test program to test the wiringPi functions
 *	PWM test
 */

#include <wiringPi.h>
#include <wiringSerial.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

int main (void)
{
int handle = serialOpen ("/dev/ttyAMA0", 19200); //115200) ;
serialParity(handle,1);
  while(1) {
    serialPutchar(handle,0b10100000);
	usleep (800);
  }

  return 0 ;
}
