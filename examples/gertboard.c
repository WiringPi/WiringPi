
/*
 * gertboard.c:
 *	Simple test for the SPI bus on the Gertboard
 *
 *	Hardware setup:
 *		D/A port 0 jumpered to A/D port 0.
 *
 *	We output a sine wave on D/A port 0 and sample A/D port 0. We then
 *	copy this value to D/A port 1 and use a 'scope on both D/A ports
 *	to check all's well.
 *
 */

#include <stdio.h>
#include <stdint.h>
//#include <stdlib.h>
#include <math.h>

#include <wiringPi.h>
#include <gertboard.h>

int main (void)
{
  int    angle ;
  int      h1 ;
  uint32_t x1 ;

  printf ("Raspberry Pi Gertboard SPI test program\n") ;

  if (gertboardSPISetup () == -1)
    return 1 ;

  for (;;)
  {
    for (angle = 0 ; angle < 360 ; ++angle)
    {
      h1 = (int)rint (sin ((double)angle * M_PI / 180.0) * 127.0 + 128.0) ;
      gertboardAnalogWrite (0, h1) ;

      x1 = gertboardAnalogRead (0) ;
      gertboardAnalogWrite (1, x1 >> 2) ;	// 10-bit A/D, 8-bit D/A
    }
  }

  return 0 ;
}
