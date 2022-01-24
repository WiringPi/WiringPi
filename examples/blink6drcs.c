/*
 * blink6drcs.c:
 *	Simple sequence over 6 pins on a remote DRC board.
 *	Aimed at the Gertduino, but it's fairly generic.
 *	This version uses DRC to talk to the ATmega on the Gertduino
 *
 * Copyright (c) 2012-2014 Gordon Henderson.
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
  GERT_BASE + 6, GERT_BASE + 5, GERT_BASE + 3, GERT_BASE + 10, GERT_BASE + 9, GERT_BASE + 13,
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
   4, 0, 1,
   5, 0, 1,

   0, 0, 1,	// Extra delay

// Back again

               5, 1, 1,
               4, 1, 1,
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

  printf ("Raspberry Pi - 6-LED Sequence\n") ;
  printf ("=============================\n") ;
  printf ("\n") ;
  printf ("  Use the 2 buttons to temporarily speed up the sequence\n") ;

  wiringPiSetupSys  () ;	// Not using the Pi's GPIO here
  drcSetupSerial (GERT_BASE, 20, "/dev/ttyAMA0", 115200) ;

  for (pin = 0 ; pin < 6 ; ++pin)
    pinMode (pinMap [pin], OUTPUT) ;

  pinMode (GERT_BASE + 16, INPUT) ;	// Buttons
  pinMode (GERT_BASE + 17, INPUT) ;

  pullUpDnControl (GERT_BASE + 16, PUD_UP) ;
  pullUpDnControl (GERT_BASE + 17, PUD_UP) ;

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
    delay        (d * digitalRead (GERT_BASE + 16) * 15 + digitalRead (GERT_BASE + 17) * 20) ;
  }

  return 0 ;
}
