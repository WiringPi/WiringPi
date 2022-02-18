/*
 * mcp23s17.c:
 *	Extend wiringPi with the MCP 23s17 SPI GPIO expander chip
 *	Copyright (c) 2013 Gordon Henderson
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
#include <stdint.h>

#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "mcp23x0817.h"

#include "mcp23s17.h"

#define	MCP_SPEED	4000000



/*
 * writeByte:
 *	Write a byte to a register on the MCP23s17 on the SPI bus.
 *********************************************************************************
 */

static void writeByte (uint8_t spiPort, uint8_t devId, uint8_t reg, uint8_t data)
{
  uint8_t spiData [4] ;

  spiData [0] = CMD_WRITE | ((devId & 7) << 1) ;
  spiData [1] = reg ;
  spiData [2] = data ;

  wiringPiSPIDataRW (spiPort, spiData, 3) ;
}

/*
 * readByte:
 *	Read a byte from a register on the MCP23s17 on the SPI bus.
 *********************************************************************************
 */

static uint8_t readByte (uint8_t spiPort, uint8_t devId, uint8_t reg)
{
  uint8_t spiData [4] ;

  spiData [0] = CMD_READ | ((devId & 7) << 1) ;
  spiData [1] = reg ;

  wiringPiSPIDataRW (spiPort, spiData, 3) ;

  return spiData [2] ;
}


/*
 * myPinMode:
 *********************************************************************************
 */

static void myPinMode (struct wiringPiNodeStruct *node, int pin, int mode)
{
  int mask, old, reg ;

  pin -= node->pinBase ;

  if (pin < 8)		// Bank A
    reg  = MCP23x17_IODIRA ;
  else
  {
    reg  = MCP23x17_IODIRB ;
    pin &= 0x07 ;
  }

  mask = 1 << pin ;
  old  = readByte (node->data0, node->data1, reg) ;

  if (mode == OUTPUT)
    old &= (~mask) ;
  else
    old |=   mask ;

  writeByte (node->data0, node->data1, reg, old) ;
}


/*
 * myPullUpDnControl:
 *********************************************************************************
 */

static void myPullUpDnControl (struct wiringPiNodeStruct *node, int pin, int mode)
{
  int mask, old, reg ;

  pin -= node->pinBase ;

  if (pin < 8)		// Bank A
    reg  = MCP23x17_GPPUA ;
  else
  {
    reg  = MCP23x17_GPPUB ;
    pin &= 0x07 ;
  }

  mask = 1 << pin ;
  old  = readByte (node->data0, node->data1, reg) ;

  if (mode == PUD_UP)
    old |=   mask ;
  else
    old &= (~mask) ;

  writeByte (node->data0, node->data1, reg, old) ;
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

    writeByte (node->data0, node->data1, MCP23x17_GPIOA, old) ;
    node->data2 = old ;
  }
  else				// Bank B
  {
    old = node->data3 ;

    if (value == LOW)
      old &= (~bit) ;
    else
      old |=   bit ;

    writeByte (node->data0, node->data1, MCP23x17_GPIOB, old) ;
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
    gpio  = MCP23x17_GPIOA ;
  else
  {
    gpio  = MCP23x17_GPIOB ;
    pin  &= 0x07 ;
  }

  mask  = 1 << pin ;
  value = readByte (node->data0, node->data1, gpio) ;

  if ((value & mask) == 0)
    return LOW ;
  else 
    return HIGH ;
}


/*
 * mcp23s17Setup:
 *	Create a new instance of an MCP23s17 SPI GPIO interface. We know it
 *	has 16 pins, so all we need to know here is the SPI address and the
 *	user-defined pin base.
 *********************************************************************************
 */

int mcp23s17Setup (const int pinBase, const int spiPort, const int devId)
{
  struct wiringPiNodeStruct *node ;

  if (wiringPiSPISetup (spiPort, MCP_SPEED) < 0)
    return FALSE ;

  writeByte (spiPort, devId, MCP23x17_IOCON,  IOCON_INIT | IOCON_HAEN) ;
  writeByte (spiPort, devId, MCP23x17_IOCONB, IOCON_INIT | IOCON_HAEN) ;

  node = wiringPiNewNode (pinBase, 16) ;

  node->data0           = spiPort ;
  node->data1           = devId ;
  node->pinMode         = myPinMode ;
  node->pullUpDnControl = myPullUpDnControl ;
  node->digitalRead     = myDigitalRead ;
  node->digitalWrite    = myDigitalWrite ;
  node->data2           = readByte (spiPort, devId, MCP23x17_OLATA) ;
  node->data3           = readByte (spiPort, devId, MCP23x17_OLATB) ;

  return TRUE ;
}
