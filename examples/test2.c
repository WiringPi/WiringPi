
/*
 * test2.c:
 *	Simple test program to test the wiringPi functions
 *	PWM test
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main (void)
{
  int bright ;

  printf ("Raspberry Pi wiringPi PWM test program\n") ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  pinMode (1, PWM_OUTPUT) ;

  for (;;)
  {
    for (bright = 0 ; bright < 1024 ; ++bright)
    {
      pwmWrite (1, bright) ;
      delay (1) ;
    }

    for (bright = 1023 ; bright >= 0 ; --bright)
    {
      pwmWrite (1, bright) ;
      delay (1) ;
    }
  }

  return 0 ;
}
