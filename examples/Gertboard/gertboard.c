/*
 * gertboard.c:
 *	Simple test for the SPI bus on the Gertboard
 *
 *	Hardware setup:
 *		D/A port 0 jumpered to A/D port 0.
 *
 *	We output a sine wave on D/A port 0 and sample A/D port 0. We then
 *	plot the input value on the terminal as a sort of vertical scrolling
 *	oscilloscipe.
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
#include <sys/ioctl.h>
#include <stdlib.h>
#include <math.h>

// Gertboard D to A is an 8-bit unit.

#define	B_SIZE	256

#include <wiringPi.h>
#include <gertboard.h>

int main (void)
{
  double angle ;
  int i, inputValue ;
  int  buffer [B_SIZE] ;
  int   cols ;
  struct winsize w ;


  printf ("Raspberry Pi Gertboard SPI test program\n") ;
  printf ("=======================================\n") ;

  ioctl (fileno (stdin), TIOCGWINSZ, &w);
  cols = w.ws_col - 2 ;

// Always initialise wiringPi. Use wiringPiSys() if you don't need
//	(or want) to run as root

  wiringPiSetupSys () ;

// Initialise the Gertboard analog hardware at pin 100

  gertboardAnalogSetup (100) ;

// Generate a Sine Wave and store in our buffer

  for (i = 0 ; i < B_SIZE ; ++i)
  {
    angle = ((double)i / (double)B_SIZE) * M_PI * 2.0 ;
    buffer [i] = (int)rint ((sin (angle)) * 127.0 + 128.0) ;
  }

// Loop, output the sine wave on analog out port 0, read it into A-D port 0
//	and display it on the screen

  for (;;)
  {
    for (i = 0 ; i < B_SIZE ; ++i)
    {
      analogWrite (100, buffer [i]) ;

      inputValue = analogRead (100) ;

// We don't need to wory about the scale or sign - the analog hardware is
//	a 10-bit value, so 0-1023. Just scale this to our terminal

      printf ("%*s\n", (inputValue * cols) / 1023, "*") ;
      delay (2) ;
    }
  }

  return 0 ;
}
