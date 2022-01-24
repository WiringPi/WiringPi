/*
 * blink12drcs.c:
 *	Simple sequence over the first 12 GPIO pins - LEDs
 *	Aimed at the Gertboard, but it's fairly generic.
 *	This version uses DRC to talk to the ATmega on the Gertboard
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/WiringPi/WiringPi
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
#include <drcSerial.h>

#define	GERT_BASE	100

static int pinMap [] =
{
  0, 1, 2, 3,		// Pi Native
  GERT_BASE + 2, GERT_BASE + 3, GERT_BASE + 4, GERT_BASE + 5,
  GERT_BASE + 6, GERT_BASE + 7, GERT_BASE + 8, GERT_BASE + 9,
} ;

// Simple sequencer data
//	Triplets of LED, On/Off and delay


int data [] =
{
               0, 1, 1,
               1, 1, 1,
   0, 0, 0,    2, 1, 1,
   1, 0, 0,    3, 1, 1,
   2, 0, 0,    4, 1, 1,
   3, 0, 0,    5, 1, 1,
   4, 0, 0,    6, 1, 1,
   5, 0, 0,    7, 1, 1,
   6, 0, 0,    8, 1, 1,
   7, 0, 0,    9, 1, 1,
   8, 0, 0,   10, 1, 1,
   9, 0, 0,   11, 1, 1,
  10, 0, 1,
  11, 0, 1,

  0, 0, 1,	// Extra delay

// Back again

              11, 1, 1,
              10, 1, 1,
  11, 0, 0,    9, 1, 1,
  10, 0, 0,    8, 1, 1,
   9, 0, 0,    7, 1, 1,
   8, 0, 0,    6, 1, 1,
   7, 0, 0,    5, 1, 1,
   6, 0, 0,    4, 1, 1,
   5, 0, 0,    3, 1, 1,
   4, 0, 0,    2, 1, 1,
   3, 0, 0,    1, 1, 1,
   2, 0, 0,    0, 1, 1,
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
  printf ("Connect LEDs up to the first 4 Pi pins and 8 pins on the ATmega\n") ;
  printf ("    from PD2 through PB1 in that order,\n") ;
  printf ("  then sit back and watch the show!\n") ;

  wiringPiSetup  () ;
  drcSetupSerial (GERT_BASE, 20, "/dev/ttyAMA0", 115200) ;

  for (pin = 0 ; pin < 12 ; ++pin)
    pinMode (pinMap [pin], OUTPUT) ;

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

    digitalWrite (pinMap [l], s) ;
    delay        (d * analogRead (GERT_BASE) / 4) ;
  }

  return 0 ;
}
