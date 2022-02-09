/*
 * reaction.c:
 *	Simple test for the PiFace interface board.
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
#include <stdlib.h>
#include <stdint.h>

#include <wiringPi.h>
#include <piFace.h>


int outputs [4] = { 0,0,0,0 } ;

#define	PIFACE	200

/*
 * light:
 *	Light up the given LED - actually lights up a pair
 *********************************************************************************
 */

void light (int led, int value)
{
  led *= 2 ;
  digitalWrite (PIFACE + led + 0, value) ;
  digitalWrite (PIFACE + led + 1, value) ;
}

/*
 * lightAll:
 *	All On or Off
 *********************************************************************************
 */

void lightAll (int onoff)
{
  light (0, onoff) ;
  light (1, onoff) ;
  light (2, onoff) ;
  light (3, onoff) ;
}


/*
 * waitForNoButtons:
 *	Wait for all buttons to be released
 *********************************************************************************
 */

void waitForNoButtons (void)
{
  int i, button ;

  for (;;)
  {
    button = 0 ;
    for (i = 0 ; i < 4 ; ++i)
      button += digitalRead (PIFACE + i) ;

    if (button == 4)
      break ;
  }
}


void scanButton (int button)
{
  if (digitalRead (PIFACE + button) == LOW)
  {
    outputs [button] ^= 1 ;
    digitalWrite (PIFACE + button, outputs [button]) ;
  }

  while (digitalRead (PIFACE + button) == LOW)
    delay (1) ;
}


int main (void)
{
  int i, j ;
  int led, button ;
  unsigned int start, stop ;

  printf ("Raspberry Pi PiFace Reaction Timer\n") ;
  printf ("==================================\n") ;

  if (piFaceSetup (PIFACE) == -1)
    exit (1) ;

// Enable internal pull-ups

  for (i = 0 ; i < 8 ; ++i)
    pullUpDnControl (PIFACE + i, PUD_UP) ;


// Main game loop:
//	Put some random LED pairs up for a few seconds, then blank ...

  for (;;)
  {
    printf ("Press any button to start ... \n") ; fflush (stdout) ;

    for (;;)
    {
      led = rand () % 4 ;
      light (led, 1) ;
      delay (10) ;
      light (led, 0) ;

      button = 0 ;
      for (j = 0 ; j < 4 ; ++j)
	button += digitalRead (PIFACE + j) ;

      if (button != 4)
	break ;
    }

    waitForNoButtons () ;

    printf ("Wait for it ... ") ; fflush (stdout) ;

    led = rand () % 4 ;
    delay (rand () % 500 + 1000) ;
    light (led, 1) ;

    start = millis () ;
    for (button = -1 ; button == -1 ; )
    {
      for (j = 0 ; j < 4 ; ++j)
	if (digitalRead (PIFACE + j) == 0)	// Pushed
	{
	  button = j ;
	  break ;
	}
    }
    stop = millis () ;
    button = 3 - button ; // Correct for the buttons/LEDs reversed

    light (led, 0) ;

    waitForNoButtons () ;

    light (led, 1) ;

    if (button == led)
    {
      printf ("You got it in %3d mS\n", stop - start) ;
    }
    else
    {
      printf ("Missed: You pushed %d - LED was %d\n", button, led) ;
      for (;;)
      {
	light (button, 1) ;
	delay (100) ;
	light (button, 0) ;
	delay (100) ;
	i = 0 ;
	for (j = 0 ; j < 4 ; ++j)
	  i += digitalRead (PIFACE + j) ;
	if (i != 4)
	  break ;
      }

      waitForNoButtons () ;
    }
    light (led, 0) ;
    delay (4000) ;
  }

  return 0 ;
}
