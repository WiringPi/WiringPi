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
#include <pthread.h>

#include "wiringPi.h"
#include "softTone.h"

#define	MAX_PINS	64

#define	PULSE_TIME	100

static int freqs         [MAX_PINS] ;
static pthread_t threads [MAX_PINS] ;

static int newPin = -1 ;


/*
 * softToneThread:
 *	Thread to do the actual PWM output
 *********************************************************************************
 */

static PI_THREAD (softToneThread)
{
  int pin, freq, halfPeriod ;
  struct sched_param param ;

  param.sched_priority = sched_get_priority_max (SCHED_RR) ;
  pthread_setschedparam (pthread_self (), SCHED_RR, &param) ;

  pin    = newPin ;
  newPin = -1 ;

  piHiPri (50) ;

  for (;;)
  {
    freq = freqs [pin] ;
    if (freq == 0)
      delay (1) ;
    else
    {
      halfPeriod = 500000 / freq ;

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

void softToneWrite (int pin, int freq)
{
  pin &= 63 ;

  /**/ if (freq < 0)
    freq = 0 ;
  else if (freq > 5000)	// Max 5KHz
    freq = 5000 ;

  freqs [pin] = freq ;
}


/*
 * softToneCreate:
 *	Create a new tone thread.
 *********************************************************************************
 */

int softToneCreate (int pin)
{
  int res ;
  pthread_t myThread ;

  pinMode      (pin, OUTPUT) ;
  digitalWrite (pin, LOW) ;

  if (threads [pin] != 0)
    return -1 ;

  freqs [pin] = 0 ;

  newPin = pin ;
  res    = pthread_create (&myThread, NULL, softToneThread, NULL) ;

  while (newPin != -1)
    delay (1) ;

  threads [pin] = myThread ;

  return res ;
}


/*
 * softToneStop:
 *	Stop an existing softTone thread
 *********************************************************************************
 */

void softToneStop (int pin)
{
  if (threads [pin] != 0)
  {
    pthread_cancel (threads [pin]) ;
    pthread_join   (threads [pin], NULL) ;
    threads [pin] = 0 ;
    digitalWrite (pin, LOW) ;
  }
}
