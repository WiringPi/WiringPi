/*
 * mcp23016.c:
 *	Extend wiringPi with the MCP 23016 I2C GPIO expander chip
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

#include <stdio.h>
#include <pthread.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "mcp23016.h"

#include "mcp23016reg.h"


/*
 * myPinMode:
 *********************************************************************************
 */

static void myPinMode (struct wiringPiNodeStruct *node, int pin, int mode)
{
  int mask, old, reg ;

  pin -= node->pinBase ;

  if (pin < 8)		// Bank A
    reg  = MCP23016_IODIR0 ;
  else
  {
    reg  = MCP23016_IODIR1 ;
    pin &= 0x07 ;
  }

  mask = 1 << pin ;
  old  = wiringPiI2CReadReg8 (node->fd, reg) ;

  if (mode == OUTPUT)
    old &= (~mask) ;
  else
    old |=   mask ;

  wiringPiI2CWriteReg8 (node->fd, reg, old) ;
}


/*
 * myDigitalWrite:
 *********************************************************************************
 */

static void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  int bit, old ;

  pin -= node->pinBase ;	// Pin now 0-15

  bit = 1 << (pin & 7) ;

  if (pin < 8)			// Bank A
  {
    old = node->data2 ;

    if (value == LOW)
      old &= (~bit) ;
    else
      old |=   bit ;

    wiringPiI2CWriteReg8 (node->fd, MCP23016_GP0, old) ;
    node->data2 = old ;
  }
  else				// Bank B
  {
    old = node->data3 ;

    if (value == LOW)
      old &= (~bit) ;
    else
      old |=   bit ;

    wiringPiI2CWriteReg8 (node->fd, MCP23016_GP1, old) ;
    node->data3 = old ;
  }
}


/*
 * myDigitalRead:
 *********************************************************************************
 */

static int myDigitalRead (struct wiringPiNodeStruct *node, int pin)
{
  int mask, value, gpio ;

  pin -= node->pinBase ;

  if (pin < 8)		// Bank A
    gpio  = MCP23016_GP0 ;
  else
  {
    gpio  = MCP23016_GP1 ;
    pin  &= 0x07 ;
  }

  mask  = 1 << pin ;
  value = wiringPiI2CReadReg8 (node->fd, gpio) ;

  if ((value & mask) == 0)
    return LOW ;
  else 
    return HIGH ;
}


/*
 * mcp23016Setup:
 *	Create a new instance of an MCP23016 I2C GPIO interface. We know it
 *	has 16 pins, so all we need to know here is the I2C address and the
 *	user-defined pin base.
 *********************************************************************************
 */

int mcp23016Setup (const int pinBase, const int i2cAddress)
{
  int fd ;
  struct wiringPiNodeStruct *node ;

  if ((fd = wiringPiI2CSetup (i2cAddress)) < 0)
    return fd ;

  wiringPiI2CWriteReg8 (fd, MCP23016_IOCON0, IOCON_INIT) ;
  wiringPiI2CWriteReg8 (fd, MCP23016_IOCON1, IOCON_INIT) ;

  node = wiringPiNewNode (pinBase, 16) ;

  node->fd              = fd ;
  node->pinMode         = myPinMode ;
  node->digitalRead     = myDigitalRead ;
  node->digitalWrite    = myDigitalWrite ;
  node->data2           = wiringPiI2CReadReg8 (fd, MCP23016_OLAT0) ;
  node->data3           = wiringPiI2CReadReg8 (fd, MCP23016_OLAT1) ;

  return 0 ;
}
