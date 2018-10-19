/*
 * htu21d.c:
 *	Extend wiringPi with the HTU21D I2C humidity and Temperature
 *	sensor. This is used in the Pi Weather station.
 *	Copyright (c) 2016 Gordon Henderson
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

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"

#include "htu21d.h"

#define	DEBUG
#undef	FAKE_SENSOR

#define	I2C_ADDRESS	0x40

int checksum (UNU uint8_t data [4])
{
  return TRUE ;
}



/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  int chan = pin - node->pinBase ;
  int fd   = node->fd ;
  uint8_t data [4] ;
  uint32_t sTemp, sHumid ;
  double   fTemp, fHumid ;
  int      cTemp, cHumid ;

  /**/ if (chan == 0)	// Read Temperature
  {

// Send read temperature command:

    data [0] = 0xF3 ;
    if (write (fd, data, 1) != 1)
      return -9999 ;

// Wait then read the data

    delay (50) ;
    if (read (fd, data, 3) != 3)
      return -9998 ;

    if (!checksum (data))
      return -9997 ;

// Do the calculation

    sTemp = (data [0] << 8) | data [1] ;
    fTemp = -48.85 + 175.72 * (double)sTemp / 63356.0 ;
    cTemp = (int)rint (((100.0 * fTemp) + 0.5) / 10.0) ;
    return cTemp ;
  }
  else if (chan == 1)	// humidity
  {
// Send read humidity command:

    data [0] = 0xF5 ;
    if (write (fd, data, 1) != 1)
      return -9999 ;

// Wait then read the data

    delay (50) ;
    if (read (fd, data, 3) != 3)
      return -9998 ;

    if (!checksum (data))
      return -9997 ;

    sHumid = (data [0] << 8) | data [1] ;
    fHumid = -6.0 + 125.0 * (double)sHumid / 65536.0 ;
    cHumid = (int)rint (((100.0 * fHumid) + 0.5) / 10.0) ;
    return cHumid ;
  }
  else
    return -9999 ;
}


/*
 * htu21dSetup:
 *	Create a new instance of a HTU21D I2C GPIO interface.
 *	This chip has a fixed I2C address, so we are not providing any
 *	allowance to change this.
 *********************************************************************************
 */

int htu21dSetup (const int pinBase)
{
  int fd ;
  struct wiringPiNodeStruct *node ;
  uint8_t data ;
  int status ;

  if ((fd = wiringPiI2CSetup (I2C_ADDRESS)) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, 2) ;

  node->fd         = fd ;
  node->analogRead = myAnalogRead ;

// Send a reset code to it:

  data = 0xFE ;
  if (write (fd, &data, 1) != 1)
    return FALSE ;

  delay (15) ;

// Read the status register to check it's really there

  status = wiringPiI2CReadReg8 (fd, 0xE7) ;

  return (status == 0x02) ? TRUE : FALSE ;
}
