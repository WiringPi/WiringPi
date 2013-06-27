/*
 * max31855.c:
 *	Extend wiringPi with the max31855 SPI Analog to Digital convertor
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

#include "max31855.h"

/*
 * myAnalogRead:
 *	Return the analog value of the given pin
 *	Note: The chip really only has one read "channel", but we're faking it
 *	here so we can read the error registers. Channel 0 will be the data
 *	channel, and 1 is the error register code.
 *	Note: Temperature returned is temp in C * 4, so divide result by 4
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  unsigned int spiData ;
  int temp ;
  int chan = pin - node->pinBase ;

  wiringPiSPIDataRW (node->fd, (unsigned char *)&spiData, 4) ;

  if (chan == 0)			// Read temp in C
  {
    spiData >>= 18 ;
    temp = spiData & 0x3FFF ;		// Bottom 13 bits
    if ((spiData & 0x2000) != 0)	// Negative
      temp = -temp ;
    return temp ;
  }
  else					// Return error bits
    return spiData & 0x7 ;
}


/*
 * max31855Setup:
 *	Create a new wiringPi device node for an max31855 on the Pi's
 *	SPI interface.
 *********************************************************************************
 */

int max31855Setup (const int pinBase, int spiChannel)
{
  struct wiringPiNodeStruct *node ;

  if (wiringPiSPISetup (spiChannel, 5000000) < 0)	// 5MHz - prob 4 on the Pi
    return -1 ;

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd         = spiChannel ;
  node->analogRead = myAnalogRead ;

  return 0 ;
}
