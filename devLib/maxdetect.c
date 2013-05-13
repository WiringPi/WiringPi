/*
 * maxdetect.c:
 *	Driver for the MaxDetect series sensors
 *
 * Copyright (c) 2013 Gordon Henderson.
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

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>

#include <wiringPi.h>

#include "maxdetect.h"

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif


/*
 * maxDetectLowHighWait:
 *	Wait for a transition from high to low on the bus
 *********************************************************************************
 */

static void maxDetectLowHighWait (const int pin)
{
  unsigned int timeOut = millis () + 2000 ;

  while (digitalRead (pin) == HIGH)
    if (millis () > timeOut)
      return ;

  while (digitalRead (pin) == LOW)
    if (millis () > timeOut)
      return ;
}


/*
 * maxDetectClockByte:
 *	Read in a single byte from the MaxDetect bus
 *********************************************************************************
 */

static unsigned int maxDetectClockByte (const int pin)
{
  unsigned int byte = 0 ;
  int bit ;

  for (bit = 0 ; bit < 8 ; ++bit)
  {
    maxDetectLowHighWait (pin) ;

// bit starting now - we need to time it.

    delayMicroseconds (30) ;
    byte <<= 1 ;
    if (digitalRead (pin) == HIGH)	// It's a 1
      byte |= 1 ;
  }

  return byte ;
}


/*
 * maxDetectRead:
 *	Read in and return the 4 data bytes from the MaxDetect sensor.
 *	Return TRUE/FALSE depending on the checksum validity
 *********************************************************************************
 */

int maxDetectRead (const int pin, unsigned char buffer [4])
{
  int i ;
  unsigned int checksum ;
  unsigned char localBuf [5] ;

// Wake up the RHT03 by pulling the data line low, then high
//	Low for 10mS, high for 40uS.

  pinMode      (pin, OUTPUT) ;
  digitalWrite (pin, 0) ; delay             (10) ;
  digitalWrite (pin, 1) ; delayMicroseconds (40) ;
  pinMode      (pin, INPUT) ;

// Now wait for sensor to pull pin low

  maxDetectLowHighWait (pin) ;

// and read in 5 bytes (40 bits)

  for (i = 0 ; i < 5 ; ++i)
    localBuf [i] = maxDetectClockByte (pin) ;

  checksum = 0 ;
  for (i = 0 ; i < 4 ; ++i)
  {
    buffer [i] = localBuf [i] ;
    checksum += localBuf [i] ;
  }
  checksum &= 0xFF ;

  return checksum == localBuf [4] ;
}


/*
 * readRHT03:
 *	Read the Temperature & Humidity from an RHT03 sensor
 *********************************************************************************
 */

int readRHT03 (const int pin, int *temp, int *rh)
{
  static unsigned int nextTime   = 0 ;
  static          int lastTemp   = 0 ;
  static          int lastRh     = 0 ;
  static          int lastResult = TRUE ;

  unsigned char buffer [4] ;

// Don't read more than once a second

  if (millis () < nextTime)
  {
    *temp = lastTemp ;
    *rh   = lastRh ;
    return lastResult ;
  }
  
  lastResult = maxDetectRead (pin, buffer) ;

  if (lastResult)
  {
    *temp      = lastTemp   = (buffer [2] * 256 + buffer [3]) ;
    *rh        = lastRh     = (buffer [0] * 256 + buffer [1]) ;
    nextTime   = millis () + 2000 ;
    return TRUE ;
  }
  else
  {
    return FALSE ;
  }
}
