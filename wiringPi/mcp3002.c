/*
 * mcp3002.c:
 *	Extend wiringPi with the MCP3002 SPI Analog to Digital convertor
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
#include <wiringPiSPI.h>

#include "mcp3002.h"

/*
 * myAnalogRead:
 *	Return the analog value of the given pin
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  unsigned char spiData [2] ;
  unsigned char chanBits ;
  int chan = pin - node->pinBase ;

  if (chan == 0)
    chanBits = 0b11010000 ;
  else
    chanBits = 0b11110000 ;

  spiData [0] = chanBits ;
  spiData [1] = 0 ;

  wiringPiSPIDataRW (node->fd, spiData, 2) ;

  return ((spiData [0] << 7) | (spiData [1] >> 1)) & 0x3FF ;
}


/*
 * mcp3002Setup:
 *	Create a new wiringPi device node for an mcp3002 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int mcp3002Setup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;

  if (wiringPiSPISetup (spiChannel, 1000000) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd         = spiChannel ;
  node->analogRead = myAnalogRead ;

  return TRUE ;
}
