/*
 * piFace.:
 *	This file to interface with the PiFace peripheral device which
 *	has an MCP23S17 GPIO device connected via the SPI bus.
 *
 *	Copyright (c) 2012-2013 Gordon Henderson
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

#include <wiringPi.h>
#include <mcp23s17.h>

#include "piFace.h"


/*
 * myDigitalWrite:
 *	Perform the digitalWrite function on the PiFace board
 *********************************************************************************
 */

void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  digitalWrite (pin + 16, value) ;
}


/*
 * myDigitalRead:
 *	Perform the digitalRead function on the PiFace board
 *	With a slight twist - if we read from base + 8, then we
 *	read from the output latch...
 *********************************************************************************
 */

int myDigitalRead (struct wiringPiNodeStruct *node, int pin)
{
  if ((pin - node->pinBase) >= 8)
    return digitalRead (pin + 8) ;
  else
    return digitalRead (pin + 16 + 8) ;
}


/*
 * myPullUpDnControl:
 *	Perform the pullUpDnControl function on the PiFace board
 *********************************************************************************
 */

void myPullUpDnControl (struct wiringPiNodeStruct *node, int pin, int pud)
{
  pullUpDnControl (pin + 16 + 8, pud) ;
}


/*
 * piFaceSetup
 *	We're going to create an instance of the mcp23s17 here, then
 *	provide our own read/write routines on-top of it...
 *	The supplied PiFace code (in Pithon) treats it as an 8-bit device
 *	where you write the output ports and read the input port using the
 *	same pin numbers, however I have had a request to be able to read
 *	the output port, so reading 8..15 will read the output latch.
 *********************************************************************************
 */

int piFaceSetup (const int pinBase)
{
  int    i ;
  struct wiringPiNodeStruct *node ;

// Create an mcp23s17 instance:

   mcp23s17Setup (pinBase + 16, 0, 0) ;

// Set the direction bits

  for (i = 0 ; i < 8 ; ++i)
  {
    pinMode (pinBase + 16 +     i, OUTPUT) ;	// Port A is the outputs
    pinMode (pinBase + 16 + 8 + i, INPUT) ;	// Port B inputs.
  }

  node = wiringPiNewNode (pinBase, 16) ;
  node->digitalRead     = myDigitalRead ;
  node->digitalWrite    = myDigitalWrite ;
  node->pullUpDnControl = myPullUpDnControl ;

  return 0 ;
}
