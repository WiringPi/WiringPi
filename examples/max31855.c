/*
 * max31855.c:
 *	SPI Thermocouple interface chip
 *
 * Copyright (c) 2015 Gordon Henderson.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <max31855.h>

int main (int argc, char *argv [])
{
  int i = 0 ;

  wiringPiSetup () ;
  max31855Setup (200, 0) ;
  max31855Setup (400, 1) ;

  for (;;)
  {
    if (i == 0)
    {
      printf ("+------+------+------+------++------+------+------+------+\n") ;
      printf ("| Raw  | Err  |  C   |   F  || Raw  | Err  |  C   |  F   |\n") ;
      printf ("+------+------+------+------++------+------+------+------+\n") ;
    }

    printf ("| %4d | %4d | %4d | %4d |",   analogRead (200), analogRead (201), analogRead (202), analogRead (203)) ;
    printf ("| %4d | %4d | %4d | %4d |\n", analogRead (400), analogRead (401), analogRead (402), analogRead (403)) ;
    delay (500) ;

    if (++i == 10)
      i = 0 ;

  }

}
