/*
 * buttons.c:
 *	Simple test for the PiFace interface board.
 *
 *	Read the buttons and output the same to the LEDs
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

int outputs [4] = { 0,0,0,0 } ;

// Use 200 as the pin-base for the PiFace board

#define	PIFACE_BASE	200


/*
 * scanButton:
 *	Read the guiven button - if it's pressed, then flip the state
 *	of the correspoinding output pin
 *********************************************************************************
 */

void scanButton (int button)
{
  if (digitalRead (PIFACE_BASE + button) == LOW)
  {
    outputs [button] ^= 1 ;
    digitalWrite (PIFACE_BASE + button, outputs [button]) ;
    printf ("Button %d pushed - output now: %s\n",
		button, (outputs [button] == 0) ? "Off" : "On") ;
  }

  while (digitalRead (PIFACE_BASE + button) == LOW)
    delay (1) ;
}


/*
 * start here
 *********************************************************************************
 */

int main (void)
{
  int pin, button ;

  printf ("Raspberry Pi wiringPi + PiFace test program\n") ;
  printf ("===========================================\n") ;
  printf ("\n") ;
  printf (
"This program reads the buttons and uses them to toggle the first 4\n"
"outputs. Push a button once to turn an output on, and push it again to\n"
"turn it off again.\n\n") ;

// Always initialise wiringPi. Use wiringPiSys() if you don't need
//	(or want) to run as root

  wiringPiSetupSys () ;

  piFaceSetup (PIFACE_BASE) ;

// Enable internal pull-ups & start with all off

  for (pin = 0 ; pin < 8 ; ++pin)
  {
    pullUpDnControl (PIFACE_BASE + pin, PUD_UP) ;
    digitalWrite    (PIFACE_BASE + pin, 0) ;
  }

// Loop, scanning the buttons

  for (;;)
  {
    for (button = 0 ; button < 4 ; ++button)
      scanButton (button) ;
    delay (5) ;
  }

  return 0 ;
}
