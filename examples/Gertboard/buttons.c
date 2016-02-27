/*
 * buttons.c:
 *	Read the Gertboard buttons. Each one will act as an on/off
 *	tiggle switch for 3 different LEDs
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

// Array to keep track of our LEDs

int leds [] = { 0, 0, 0 } ;

// scanButton:
//	See if a button is pushed, if so, then flip that LED and
//	wait for the button to be let-go

void scanButton (int button)
{
  if (digitalRead (button) == HIGH)	// Low is pushed
    return ;

  leds [button] ^= 1 ; // Invert state
  digitalWrite (4 + button, leds [button]) ;

  while (digitalRead (button) == LOW)	// Wait for release
    delay (10) ;
}

int main (void)
{
  int i ;

  printf ("Raspberry Pi Gertboard Button Test\n") ;

  wiringPiSetup () ;

// Setup the outputs:
//	Pins 3, 4, 5, 6 and 7 output:
//	We're not using 3 or 4, but make sure they're off anyway
//	(Using same hardware config as blink12.c)

  for (i = 3 ; i < 8 ; ++i)
  {
    pinMode      (i, OUTPUT) ;
    digitalWrite (i, 0) ;
  }

// Setup the inputs

  for (i = 0 ; i < 3 ; ++i)
  {
    pinMode         (i, INPUT) ;
    pullUpDnControl (i, PUD_UP) ;
    leds [i] = 0 ;
  }

  for (;;)
  {
    for (i = 0 ; i < 3 ; ++i)
      scanButton (i) ;
    delay (1) ;
  }
}
