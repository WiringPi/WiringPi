/*
 * max5322.c:
 *	Extend wiringPi with the MAX5322 SPI Digital to Analog convertor
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

#include "max5322.h"

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
    chanBits = 0b01000000 ;
  else
    chanBits = 0b01010000 ;

  chanBits |= ((value >> 12) & 0x0F) ;
  dataBits  = ((value      ) & 0xFF) ;

  spiData [0] = chanBits ;
  spiData [1] = dataBits ;

  wiringPiSPIDataRW (node->fd, spiData, 2) ;
}

/*
 * max5322Setup:
 *	Create a new wiringPi device node for an max5322 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int max5322Setup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;
  unsigned char spiData [2] ;

  if (wiringPiSPISetup (spiChannel, 8000000) < 0)	// 10MHz Max
    return -1 ;

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd          = spiChannel ;
  node->analogWrite = myAnalogWrite ;

// Enable both DACs

  spiData [0] = 0b11100000 ;
  spiData [1] = 0 ;
  
  wiringPiSPIDataRW (node->fd, spiData, 2) ;

  return 0 ;
}
