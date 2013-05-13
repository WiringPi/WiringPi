/*
 * mcp4802.c:
 *	Extend wiringPi with the MCP4802 SPI Digital to Analog convertor
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

#include "mcp4802.h"

/*
 * myAnalogWrite:
 *	Write analog value on the given pin
 *********************************************************************************
 */

static void myAnalogWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  unsigned char spiData [2] ;
  unsigned char chanBits, dataBits ;
  int chan = pin - node->pinBase ;

  if (chan == 0)
    chanBits = 0x30 ;
  else
    chanBits = 0xB0 ;

  chanBits |= ((value >> 4) & 0x0F) ;
  dataBits  = ((value << 4) & 0xF0) ;

  spiData [0] = chanBits ;
  spiData [1] = dataBits ;

  wiringPiSPIDataRW (node->fd, spiData, 2) ;
}

/*
 * mcp4802Setup:
 *	Create a new wiringPi device node for an mcp4802 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int mcp4802Setup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;

  if (wiringPiSPISetup (spiChannel, 1000000) < 0)
    return -1 ;

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd          = spiChannel ;
  node->analogWrite = myAnalogWrite ;

  return 0 ;
}
