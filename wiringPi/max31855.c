/*
 * max31855.c:
 *	Extend wiringPi with the max31855 SPI Analog to Digital convertor
 *	Copyright (c) 2012-2015 Gordon Henderson
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

#include <byteswap.h>
#include <stdint.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "max31855.h"

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  uint32_t spiData ;
  int temp ;
  int chan = pin - node->pinBase ;

  wiringPiSPIDataRW (node->fd, (unsigned char *)&spiData, 4) ;

  spiData = __bswap_32(spiData) ;

  switch (chan)
  {
    case 0:				// Existing read - return raw value * 4
      spiData >>= 18 ;
      temp = spiData & 0x1FFF ;		// Bottom 13 bits
      if ((spiData & 0x2000) != 0)	// Negative
        temp = -temp ;

      return temp ;

    case 1:				// Return error bits
      return spiData & 0x7 ;

    case 2:				// Return temp in C * 10
      spiData >>= 18 ;
      temp = spiData & 0x1FFF ;		// Bottom 13 bits
      if ((spiData & 0x2000) != 0)	// Negative
        temp = -temp ;

      return (int)((((double)temp * 25) + 0.5) / 10.0) ;

    case 3:				// Return temp in F * 10
      spiData >>= 18 ;
      temp = spiData & 0x1FFF ;		// Bottom 13 bits
      if ((spiData & 0x2000) != 0)	// Negative
        temp = -temp ;

      return (int)((((((double)temp * 0.25 * 9.0 / 5.0) + 32.0) * 100.0) + 0.5) / 10.0) ;

    default:				// Who knows...
      return 0 ;

  }
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

  node = wiringPiNewNode (pinBase, 4) ;

  node->fd         = spiChannel ;
  node->analogRead = myAnalogRead ;

  return 0 ;
}
