
/*
 * test1.c:
 *	Simple test program to test the wiringPi functions
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


// Simple sequencer data
//	Triplets of LED, On/Off and delay

uint8_t data [] =
{
            0, 1, 1,
            1, 1, 1,
  0, 0, 0,  2, 1, 1,
  1, 0, 0,  3, 1, 1,
  2, 0, 0,  4, 1, 1,
  3, 0, 0,  5, 1, 1,
  4, 0, 0,  6, 1, 1,
  5, 0, 0,  7, 1, 1,
  6, 0, 1,
  7, 0, 1,

  0, 0, 1,	// Extra delay

// Back again

            7, 1, 1,
            6, 1, 1,
  7, 0, 0,  5, 1, 1,
  6, 0, 0,  4, 1, 1,
  5, 0, 0,  3, 1, 1,
  4, 0, 0,  2, 1, 1,
  3, 0, 0,  1, 1, 1,
  2, 0, 0,  0, 1, 1,
  1, 0, 1,
  0, 0, 1,

  0, 0, 1,	// Extra delay

  9, 9, 9,	// End marker

} ;


int main (void)
{
  int pin ;
  int dataPtr ;
  int l, s, d ;

  printf ("Raspberry Pi wiringPi test program\n") ;

  if (wiringPiSetup () == -1)
    exit (1) ;

  for (pin = 0 ; pin < 8 ; ++pin)
    pinMode (pin, OUTPUT) ;

  pinMode (8, INPUT) ; 	// Pin 8 SDA0 - Has on-board 2k2 pull-up resistor

  dataPtr = 0 ;

  for (;;)
  {
    l = data [dataPtr++] ;	// LED
    s = data [dataPtr++] ;	// State
    d = data [dataPtr++] ;	// Duration (10ths)

    if ((l + s + d) == 27)
    {
      dataPtr = 0 ;
      continue ;
    }

    digitalWrite (l, s) ;

    if (digitalRead (8) == 0)	// Pressed as our switch shorts to ground
      delay (d * 10) ;	// Faster!
    else
      delay (d * 100) ;
  }

  return 0 ;
}
