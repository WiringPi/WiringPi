/*
 * softTone.c:
 *	Test of the softTone module in wiringPi
 *	Plays a scale out on pin 3 - connect pizeo disc to pin 3 & 0v
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/WiringPi/WiringPi
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
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softTone.h>

#define	PIN	3

int scale [8] = { 262, 294, 330, 349, 392, 440, 494, 525 } ;

int main ()
{
  int i ;

  wiringPiSetup () ;

  softToneCreate (PIN) ;

  for (;;)
  {
    for (i = 0 ; i < 8 ; ++i)
    {
      printf ("%3d\n", i) ;
      softToneWrite (PIN, scale [i]) ;
      delay (500) ;
    }
  }
}
