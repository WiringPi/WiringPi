/*
 * blink12.c:
 *	Simple sequence over the first 12 GPIO pins - LEDs
 *	Aimed at the Gertboard, but it's fairly generic.
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <wiringPi.h>

// Simple sequencer data
//	Triplets of LED, On/Off and delay

int data [] =
{
            0, 1, 1,
            1, 1, 1,
  0, 0, 0,  2, 1, 1,
  1, 0, 0,  3, 1, 1,
  2, 0, 0,  4, 1, 1,
  3, 0, 0,  5, 1, 1,
  4, 0, 0,  6, 1, 1,
  5, 0, 0,  7, 1, 1,
  6, 0, 0, 11, 1, 1,
  7, 0, 0, 10, 1, 1,
 11, 0, 0, 13, 1, 1,
 10, 0, 0, 12, 1, 1,
 13, 0, 1,
 12, 0, 1,

  0, 0, 1,	// Extra delay

// Back again

           12, 1, 1,
           13, 1, 1,
 12, 0, 0, 10, 1, 1,
 13, 0, 0, 11, 1, 1,
 10, 0, 0,  7, 1, 1,
 11, 0, 0,  6, 1, 1,
  7, 0, 0,  5, 1, 1,
  6, 0, 0,  4, 1, 1,
  5, 0, 0,  3, 1, 1,
  4, 0, 0,  2, 1, 1,
  3, 0, 0,  1, 1, 1,
  2, 0, 0,  0, 1, 1,
  1, 0, 1,
  0, 0, 1,

  0, 0, 1,	// Extra delay

  0, 9, 0,	// End marker

} ;


int main (void)
{
  int pin ;
  int dataPtr ;
  int l, s, d ;

  printf ("Raspberry Pi - 12-LED Sequence\n") ;
  printf ("==============================\n") ;
  printf ("\n") ;
  printf ("Connect LEDs up to the first 8 GPIO pins, then pins 11, 10, 13, 12 in\n") ;
  printf ("    that order, then sit back and watch the show!\n") ;

  wiringPiSetup () ;

  for (pin = 0 ; pin < 14 ; ++pin)
    pinMode (pin, OUTPUT) ;

  dataPtr = 0 ;

  for (;;)
  {
    l = data [dataPtr++] ;	// LED
    s = data [dataPtr++] ;	// State
    d = data [dataPtr++] ;	// Duration (10ths)

    if (s == 9)			// 9 -> End Marker
    {
      dataPtr = 0 ;
      continue ;
    }

    digitalWrite (l, s) ;
    delay        (d * 100) ;
  }

  return 0 ;
}
