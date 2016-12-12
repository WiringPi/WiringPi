/*
 * piFace.:
 *	Copyright (c) 2012-2016 Gordon Henderson
 *
 *	This file to interface with the PiFace peripheral device which
 *	has an MCP23S17 GPIO device connected via the SPI bus.
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
#include <stdint.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "../wiringPi/mcp23x0817.h"

#include "piFace.h"

#define	PIFACE_SPEED	4000000
#define	PIFACE_DEVNO	0



/*
 * writeByte:
 *	Write a byte to a register on the MCP23S17 on the SPI bus.
 *********************************************************************************
 */

static void writeByte (uint8_t reg, uint8_t data)
{
  uint8_t spiData [4] ;

  spiData [0] = CMD_WRITE ;
  spiData [1] = reg ;
  spiData [2] = data ;

  wiringPiSPIDataRW (PIFACE_DEVNO, spiData, 3) ;
}

/*
 * readByte:
 *	Read a byte from a register on the MCP23S17 on the SPI bus.
 *********************************************************************************
 */

static uint8_t readByte (uint8_t reg)
{
  uint8_t spiData [4] ;

  spiData [0] = CMD_READ ;
  spiData [1] = reg ;

  wiringPiSPIDataRW (PIFACE_DEVNO, spiData, 3) ;

  return spiData [2] ;
}


/*
 * myDigitalWrite:
 *	Perform the digitalWrite function on the PiFace board
 *********************************************************************************
 */

void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  uint8_t mask, old ;

  pin -= node->pinBase ;
  mask = 1 << pin ;
  old  = readByte (MCP23x17_GPIOA) ;

  if (value == 0)
    old &= (~mask) ;
  else
    old |=   mask ;

  writeByte (MCP23x17_GPIOA, old) ;
}


/*
 * myDigitalRead:
 *	Perform the digitalRead function on the PiFace board
 *********************************************************************************
 */

int myDigitalRead (struct wiringPiNodeStruct *node, int pin)
{
  uint8_t mask, reg ;

  mask = 1 << ((pin - node->pinBase) & 7) ;

  if (pin < 8)
    reg = MCP23x17_GPIOB ;	// Input regsiter
  else
    reg = MCP23x17_OLATA ;	// Output latch regsiter

  if ((readByte (reg) & mask) != 0)
    return HIGH ;
  else
    return LOW ;
}


/*
 * myPullUpDnControl:
 *	Perform the pullUpDnControl function on the PiFace board
 *********************************************************************************
 */

void myPullUpDnControl (struct wiringPiNodeStruct *node, int pin, int pud)
{
  uint8_t mask, old ;

  mask = 1 << (pin - node->pinBase) ;
  old  = readByte (MCP23x17_GPPUB) ;

  if (pud == 0)
    old &= (~mask) ;
  else
    old |=   mask ;

  writeByte (MCP23x17_GPPUB, old) ;
}


/*
 * piFaceSetup
 *	Setup the SPI interface and initialise the MCP23S17 chip
 *	We create one node with 16 pins - each if the first 8 pins being read
 *	and write - although the operations actually go to different
 *	hardware ports. The top 8 let you read the state of the output register.
 *********************************************************************************
 */

int piFaceSetup (const int pinBase)
{
  int    x ;
  struct wiringPiNodeStruct *node ;

  if ((x = wiringPiSPISetup (PIFACE_DEVNO, PIFACE_SPEED)) < 0)
    return x ;

// Setup the MCP23S17

  writeByte (MCP23x17_IOCON,  IOCON_INIT) ;
  writeByte (MCP23x17_IODIRA, 0x00) ;		// Port A -> Outputs
  writeByte (MCP23x17_IODIRB, 0xFF) ;		// Port B -> Inputs

  node = wiringPiNewNode (pinBase, 16) ;
  node->digitalRead     = myDigitalRead ;
  node->digitalWrite    = myDigitalWrite ;
  node->pullUpDnControl = myPullUpDnControl ;

  return 0 ;
}
