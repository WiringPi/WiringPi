/*
 * piglow.c:
 *	Very simple demonstration of the PiGlow board.
 *	This uses the SN3218 directly - soon there will be a new PiGlow
 *	devLib device which will handle the PiGlow board on a more easy
 *	to use manner...
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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

#include <wiringPi.h>
#include <sn3218.h>

#define	LED_BASE	533

int main (void)
{
  int i, j ;

  wiringPiSetupSys () ;

  sn3218Setup (LED_BASE) ;

  for (;;)
  {
    for (i = 0 ; i < 256 ; ++i)
      for (j = 0 ; j < 18 ; ++j)
	analogWrite (LED_BASE + j, i) ;

    for (i = 255 ; i >= 0 ; --i)
      for (j = 0 ; j < 18 ; ++j)
	analogWrite (LED_BASE + j, i) ;
  }
}
