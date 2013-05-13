/*
 * temperature.c:
 *	Demonstrate use of the Gertboard A to D converter to make
 *	a simple thermometer using the LM35.
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
#include <gertboard.h>

int main ()
{
  int x1, x2 ;
  double v1, v2 ;

  printf ("\n") ;
  printf ("Gertboard demo: Simple Thermemeter\n") ;
  printf ("==================================\n") ;

// Always initialise wiringPi. Use wiringPiSys() if you don't need
//	(or want) to run as root

  wiringPiSetupSys () ;

// Initialise the Gertboard analog hardware at pin 100

  gertboardAnalogSetup (100) ;

  printf ("\n") ;
  printf ("| Channel 0 | Channel 1 | Temperature 1 | Temperature 2 |\n") ;

  for (;;)
  {

// Read the 2 channels:

    x1 = analogRead (100) ;
    x2 = analogRead (101) ;

// Convert to a voltage:

    v1 = (double)x1 / 1023.0 * 3.3 ;
    v2 = (double)x2 / 1023.0 * 3.3 ;

// Print

    printf ("|    %6.3f |    %6.3f |", v1, v2) ;

// Print Temperature of both channels by converting the LM35 reading
//	to a temperature. Fortunately these are easy: 0.01 volts per C.

    printf ("          %4.1f |          %4.1f |\r", v1 * 100.0, v2 * 100.0) ;
    fflush (stdout) ;
  }

  return 0 ;
}

