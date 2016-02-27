/*
 * sn3218.c:
 *	Extend wiringPi with the SN3218 I2C LEd Driver
 *	Copyright (c) 2012-2013 Gordon Henderson
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

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "sn3218.h"

/*
 * myAnalogWrite:
 *	Write analog value on the given pin
 *********************************************************************************
 */

static void myAnalogWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  int fd   = node->fd ;
  int chan = 0x01 + (pin - node->pinBase) ;
  
  wiringPiI2CWriteReg8 (fd, chan, value & 0xFF) ;	// Value
  wiringPiI2CWriteReg8 (fd, 0x16, 0x00) ;		// Update
}

/*
 * sn3218Setup:
 *	Create a new wiringPi device node for an sn3218 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int sn3218Setup (const int pinBase)
{
  int fd ;
  struct wiringPiNodeStruct *node ;

  if ((fd = wiringPiI2CSetup (0x54)) < 0)
    return fd ;

// Setup the chip - initialise all 18 LEDs to off

//wiringPiI2CWriteReg8 (fd, 0x17, 0) ;		// Reset
  wiringPiI2CWriteReg8 (fd, 0x00, 1) ;		// Not Shutdown
  wiringPiI2CWriteReg8 (fd, 0x13, 0x3F) ;	// Enable LEDs  0- 5
  wiringPiI2CWriteReg8 (fd, 0x14, 0x3F) ;	// Enable LEDs  6-11
  wiringPiI2CWriteReg8 (fd, 0x15, 0x3F) ;	// Enable LEDs 12-17
  wiringPiI2CWriteReg8 (fd, 0x16, 0x00) ;	// Update
  
  node = wiringPiNewNode (pinBase, 18) ;

  node->fd          = fd ;
  node->analogWrite = myAnalogWrite ;

  return 0 ;
}
