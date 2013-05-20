/*
 * pcf8591.c:
 *	Extend wiringPi with the PCF8591 I2C GPIO Analog expander chip
 *	The chip has 1 8-bit DAC and 4 x 8-bit ADCs
 *	Copyright (c) 2013 Gordon Henderson
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

#include <unistd.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"

#include "pcf8591.h"


/*
 * myAnalogWrite:
 *********************************************************************************
 */

static void myAnalogWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  unsigned char b [2] ;
  b [0] = 0x40 ;
  b [1] = value & 0xFF ;
  write (node->fd, b, 2) ;
}


/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  int x ;

  wiringPiI2CWrite (node->fd, 0x40 | ((pin - node->pinBase) & 3)) ;

  x = wiringPiI2CRead (node->fd) ;	// Throw away the first read
  x = wiringPiI2CRead (node->fd) ;

  return x ;
}


/*
 * pcf8591Setup:
 *	Create a new instance of a PCF8591 I2C GPIO interface. We know it
 *	has 4 pins, (4 analog inputs and 1 analog output which we'll shadow
 *	input 0) so all we need to know here is the I2C address and the
 *	user-defined pin base.
 *********************************************************************************
 */

int pcf8591Setup (const int pinBase, const int i2cAddress)
{
  int fd ;
  struct wiringPiNodeStruct *node ;

  if ((fd = wiringPiI2CSetup (i2cAddress)) < 0)
    return fd ;

  node = wiringPiNewNode (pinBase, 4) ;

  node->fd          = fd ;
  node->analogRead  = myAnalogRead ;
  node->analogWrite = myAnalogWrite ;

  return 0 ;
}
