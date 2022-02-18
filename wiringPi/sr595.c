/*
 * sr595.c:
 *	Extend wiringPi with the 74x595 shift register as a GPIO
 *	expander chip.
 *	Note that the code can cope with a number of 595's
 *	daisy-chained together - up to 4 for now as we're storing
 *	the output "register" in a single unsigned int.
 *
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdint.h>

#include "wiringPi.h"

#include "sr595.h"


/*
 * myDigitalWrite:
 *********************************************************************************
 */

static void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  unsigned int mask ;
  int  dataPin, clockPin, latchPin ;
  int  bit, bits, output ;

  pin     -= node->pinBase ;				// Normalise pin number
  bits     = node->pinMax - node->pinBase + 1 ;		// ie. number of clock pulses
  dataPin  = node->data0 ;
  clockPin = node->data1 ;
  latchPin = node->data2 ;
  output   = node->data3 ;

  mask = 1 << pin ;

  if (value == LOW)
    output &= (~mask) ;
  else
    output |=   mask ;

  node->data3 = output ;

// A low -> high latch transition copies the latch to the output pins

  digitalWrite (latchPin, LOW) ; delayMicroseconds (1) ;
    for (bit = bits - 1 ; bit >= 0 ; --bit)
    {
      digitalWrite (dataPin, output & (1 << bit)) ;

      digitalWrite (clockPin, HIGH) ; delayMicroseconds (1) ;
      digitalWrite (clockPin, LOW) ;  delayMicroseconds (1) ;
    }
  digitalWrite (latchPin, HIGH) ; delayMicroseconds (1) ;
}


/*
 * sr595Setup:
 *	Create a new instance of a 74x595 shift register GPIO expander.
 *********************************************************************************
 */

int sr595Setup (const int pinBase, const int numPins,
	const int dataPin, const int clockPin, const int latchPin) 
{
  struct wiringPiNodeStruct *node ;

  node = wiringPiNewNode (pinBase, numPins) ;

  node->data0           = dataPin ;
  node->data1           = clockPin ;
  node->data2           = latchPin ;
  node->data3           = 0 ;		// Output register
  node->digitalWrite    = myDigitalWrite ;

// Initialise the underlying hardware

  digitalWrite (dataPin,  LOW) ;
  digitalWrite (clockPin, LOW) ;
  digitalWrite (latchPin, HIGH) ;

  pinMode (dataPin,  OUTPUT) ;
  pinMode (clockPin, OUTPUT) ;
  pinMode (latchPin, OUTPUT) ;

  return TRUE ;
}
