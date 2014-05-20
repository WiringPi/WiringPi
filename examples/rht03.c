/*
 * rht03.c:
 *	Driver for the MaxDetect series sensors
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>

#include <wiringPi.h>
#include <maxdetect.h>

#define	RHT03_PIN	0

/*
 ***********************************************************************
 * The main program
 ***********************************************************************
 */

int main (void)
{
  int temp, rh ;
  int newTemp, newRh ;

  temp = rh = newTemp = newRh = 0 ;

  wiringPiSetup () ;
  piHiPri       (55) ;

  for (;;)
  {
    delay (100) ;

    if (!readRHT03 (RHT03_PIN, &newTemp, &newRh))
      continue ;

    if ((temp != newTemp) || (rh != newRh))
    {
      temp = newTemp ;
      rh   = newRh ;
      if ((temp & 0x8000) != 0)	// Negative
      {
	temp &= 0x7FFF ;
	temp = -temp ;
      }
      printf ("Temp: %5.1f, RH: %5.1f%%\n", temp / 10.0, rh / 10.0) ;
    }
  }

  return 0 ;
}
