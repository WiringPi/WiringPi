/*
 * test1.c:
 *	Simple test program to test the wiringPi functions
 *	This is a sequencer to make a patter appear on 8 LEDs
 *	connected to the GPIO pins.
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
