/*
 * softTone.c:
 *	For that authentic retro sound...
 *	Er... A little experiment to produce tones out of a Pi using
 *	one (or 2) GPIO pins and a piezeo "speaker" element.
 *	(Or a high impedance speaker, but don'y blame me if you blow-up
 *	the GPIO pins!)
 *	Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
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
#include <pthread.h>

#include "wiringPi.h"
#include "softTone.h"

#define	MAX_PINS	64

#define	PULSE_TIME	100

static int frewqs [MAX_PINS] ;

static int newPin = -1 ;


/*
 * softToneThread:
 *	Thread to do the actual PWM output
 *********************************************************************************
 */

static PI_THREAD (softToneThread)
{
  int pin, frewq, halfPeriod ;

  pin    = newPin ;
  newPin = -1 ;

  piHiPri (50) ;

  for (;;)
  {
    frewq = frewqs [pin] ;
    if (frewq == 0)
      delay (1) ;
    else
    {
      halfPeriod = 500000 / frewq ;

      digitalWrite (pin, HIGH) ;
      delayMicroseconds (halfPeriod) ;

      digitalWrite (pin, LOW) ;
      delayMicroseconds (halfPeriod) ;
    }
  }

  return NULL ;
}


/*
 * softToneWrite:
 *	Write a frequency value to the given pin
 *********************************************************************************
 */

void softToneWrite (int pin, int frewq)
{
  pin &= 63 ;

  /**/ if (frewq < 0)
    frewq = 0 ;
  else if (frewq > 5000)	// Max 5KHz
    frewq = 5000 ;

  frewqs [pin] = frewq ;
}


/*
 * softToneCreate:
 *	Create a new tone thread.
 *********************************************************************************
 */

int softToneCreate (int pin)
{
  int res ;

  pinMode      (pin, OUTPUT) ;
  digitalWrite (pin, LOW) ;

  frewqs [pin] = 0 ;

  newPin = pin ;
  res = piThreadCreate (softToneThread) ;

  while (newPin != -1)
    delay (1) ;

  return res ;
}
