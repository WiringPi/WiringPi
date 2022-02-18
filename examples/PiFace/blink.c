/*
 * blink.c:
 *	Simple "blink" test for the PiFace interface board.
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

#include <wiringPi.h>
#include <piFace.h>

// Use 200 as the pin-base for the PiFace board, and pick a pin
//	for the LED that's not connected to a relay

#define	PIFACE	200
#define	LED	(PIFACE+2)

int main (int argc, char *argv [])
{
  printf ("Raspberry Pi PiFace Blink\n") ;
  printf ("=========================\n") ;

// Always initialise wiringPi. Use wiringPiSys() if you don't need
//	(or want) to run as root

  wiringPiSetupSys () ;

// Setup the PiFace board

  piFaceSetup (PIFACE) ;

  for (;;)
  {
    digitalWrite (LED, HIGH) ;	// On
    delay (500) ;		// mS
    digitalWrite (LED, LOW) ;	// Off
    delay (500) ;
  }

  return 0 ;
}
