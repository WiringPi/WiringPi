/*
 * rht03.c:
 *	Extend wiringPi with the rht03 Maxdetect 1-Wire sensor.
 *	Copyright (c) 2016-2017 Gordon Henderson
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

#include <sys/time.h>
#include <stdio.h>
#include <stdio.h>
#include <time.h>

#include "wiringPi.h"
#include "rht03.h"

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

static int maxDetectRead (const int pin, unsigned char buffer [4])
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
 * myReadRHT03:
 *	Read the Temperature & Humidity from an RHT03 sensor
 *	Values returned are *10, so 123 is 12.3.
 *********************************************************************************
 */

static int myReadRHT03 (const int pin, int *temp, int *rh)
{
  int result ;
  unsigned char buffer [4] ;

// Read ...
  
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

  return TRUE ;
}


/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  int piPin = node->fd ;
  int chan  = pin - node->pinBase ;
  int temp  = -9997 ;
  int rh    = -9997 ;
  int try ;

  if (chan > 1)
    return -9999 ;	// Bad parameters

  for (try = 0 ; try < 10 ; ++try)
  {
    if (myReadRHT03 (piPin, &temp, &rh))
      return chan == 0 ? temp : rh ;
  }

  return -9998 ;
}


/*
 * rht03Setup:
 *	Create a new instance of an RHT03 temperature sensor.
 *********************************************************************************
 */

int rht03Setup (const int pinBase, const int piPin)
{
  struct wiringPiNodeStruct *node ;

  if ((piPin & PI_GPIO_MASK) != 0)	// Must be an on-board pin
    return FALSE ;
  
// 2 pins - temperature and humidity

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd         = piPin ;
  node->analogRead = myAnalogRead ;

  return TRUE ;
}
