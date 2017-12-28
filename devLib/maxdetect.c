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

#include <sys/time.h>
#include <stdio.h>
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
 *	Wait for a transition from low to high on the bus
 *********************************************************************************
 */

static int maxDetectLowHighWait (const int pin)
{
  struct timeval now, timeOut, timeUp ;

// If already high then wait for pin to go low

  gettimeofday (&now, NULL) ;
  timerclear   (&timeOut) ;
  timeOut.tv_usec = 1000 ;
  timeradd     (&now, &timeOut, &timeUp) ;

  while (digitalRead (pin) == HIGH)
  {
    gettimeofday (&now, NULL) ;
    if (timercmp (&now, &timeUp, >))
      return FALSE ;
  }

// Wait for it to go HIGH

  gettimeofday (&now, NULL) ;
  timerclear (&timeOut) ;
  timeOut.tv_usec = 1000 ;
  timeradd (&now, &timeOut, &timeUp) ;

  while (digitalRead (pin) == LOW)
  {
    gettimeofday (&now, NULL) ;
    if (timercmp (&now, &timeUp, >))
      return FALSE ;
  }

  return TRUE ;
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
    if (!maxDetectLowHighWait (pin))
      return 0 ;

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
  struct timeval now, then, took ;

// See how long we took

  gettimeofday (&then, NULL) ;

// Wake up the RHT03 by pulling the data line low, then high
//	Low for 10mS, high for 40uS.

  pinMode      (pin, OUTPUT) ;
  digitalWrite (pin, 0) ; delay             (10) ;
  digitalWrite (pin, 1) ; delayMicroseconds (40) ;
  pinMode      (pin, INPUT) ;

// Now wait for sensor to pull pin low

  if (!maxDetectLowHighWait (pin))
    return FALSE ;

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

// See how long we took
  
  gettimeofday (&now, NULL) ;
  timersub (&now, &then, &took) ;

// Total time to do this should be:
//	10mS + 40µS - reset
//	+ 80µS + 80µS - sensor doing its low -> high thing
//	+ 40 * (50µS + 27µS (0) or 70µS (1) )
//	= 15010µS
// so if we take more than that, we've had a scheduling interruption and the
// reading is probably bogus.

  if ((took.tv_sec != 0) || (took.tv_usec > 16000))
    return FALSE ;

  return checksum == localBuf [4] ;
}


/*
 * readRHT03:
 *	Read the Temperature & Humidity from an RHT03 sensor
 *	Values returned are *10, so 123 is 12.3.
 *********************************************************************************
 */

int readRHT03 (const int pin, int *temp, int *rh)
{
  static struct timeval then ;	// will initialise to zero
  static        int     lastTemp = 0 ;
  static        int     lastRh   = 0 ;

  int result ;
  struct timeval now, timeOut ;
  unsigned char buffer [4] ;

// The data sheets say to not read more than once every 2 seconds, so you
//	get the last good reading

  gettimeofday (&now, NULL) ;
  if (timercmp (&now, &then, <))
  {
    *rh   = lastRh ;
    *temp = lastTemp ;
    return TRUE ;
  }

// Set timeout for next read

  gettimeofday (&now, NULL) ;
  timerclear   (&timeOut) ;
  timeOut.tv_sec = 2 ;
  timeradd (&now, &timeOut, &then) ;

// Read ...
  
  result = maxDetectRead (pin, buffer) ;

  if (!result) // Try again, but just once
    result = maxDetectRead (pin, buffer) ;

  if (!result)
    return FALSE ;

  *rh   = (buffer [0] * 256 + buffer [1]) ;
  *temp = (buffer [2] * 256 + buffer [3]) ;

  if ((*temp & 0x8000) != 0)	// Negative
  {
    *temp &= 0x7FFF ;
    *temp = -*temp ;
  }

// Discard obviously bogus readings - the checksum can't detect a 2-bit error
//	(which does seem to happen - no realtime here)

  if ((*rh > 999) || (*temp > 800) || (*temp < -400))
    return FALSE ;

  lastRh   = *rh ;
  lastTemp = *temp ;

  return TRUE ;
}
