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

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

//#include <unistd.h>
//#include <stdint.h>
#include <stdio.h>
//#include <string.h>
#include <time.h>
//#include <ctype.h>

#include "wiringPi.h"
#include "../devLib/maxdetect.h"

#include "rht03.h"


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
