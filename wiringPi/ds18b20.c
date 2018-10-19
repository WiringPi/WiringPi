/*
 * ds18b20.c:
 *	Extend wiringPi with the DS18B20 1-Wire temperature sensor.
 *	This is used in the Pi Weather Station and many other places.
 *	Copyright (c) 2016 Gordon Henderson
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>

#include "wiringPi.h"

#include "ds18b20.h"

#define	W1_PREFIX	"/sys/bus/w1/devices/28-"
#define	W1_POSTFIX	"/w1_slave"


/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  int  chan = pin - node->pinBase ;
  int  fd = node->fd ;
  char buffer [4096] ;
  char *p ;
  int  temp, sign ;

  if (chan != 0)
    return -9999 ;

// Rewind the file - we're keeping it open to keep things going
//	smoothly

  lseek (fd, 0, SEEK_SET) ;

// Read the file - we know it's only a couple of lines, so this ought to be
//	more than enough

  if (read (fd, buffer, 4096) <= 0)	// Read nothing, or it failed in some odd way
    return -9998 ;

// Look for YES, then t=

  if (strstr (buffer, "YES") == NULL)
    return -9997 ;

  if ((p = strstr (buffer, "t=")) == NULL)
    return -9996 ;

// p points to the 't', so we skip over it...

  p += 2 ;

// and extract the number
//	(without caring about overflow)


  if (*p == '-')	// Negative number?
  {
    sign = -1 ;
    ++p ;
  }
  else
    sign = 1 ;

  temp = 0 ;
  while (isdigit (*p))
  {
    temp = temp * 10 + (*p - '0') ;
    ++p ;
  }

// We know it returns temp * 1000, but we only really want temp * 10, so
//	do a bit of rounding...

  temp = (temp + 50) / 100 ;
  return temp * sign ;
}


/*
 * ds18b20Setup:
 *	Create a new instance of a DS18B20 temperature sensor.
 *********************************************************************************
 */

int ds18b20Setup (const int pinBase, const char *deviceId)
{
  int fd ;
  struct wiringPiNodeStruct *node ;
  char *fileName ;

// Allocate space for the filename

  if ((fileName = malloc (strlen (W1_PREFIX) + strlen (W1_POSTFIX) + strlen (deviceId) + 1)) == NULL)
    return FALSE ;

  sprintf (fileName, "%s%s%s", W1_PREFIX, deviceId, W1_POSTFIX) ;

  fd = open (fileName, O_RDONLY) ;

  free (fileName) ;

  if (fd < 0)
    return FALSE ;

// We'll keep the file open, to make access a little faster
//	although it's very slow reading these things anyway )-:

  node = wiringPiNewNode (pinBase, 1) ;

  node->fd         = fd ;
  node->analogRead = myAnalogRead ;

  return TRUE ;
}
