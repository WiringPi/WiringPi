/*
 * wfi.c:
 *	Wait for Interrupt test program
 *
 *	This program demonstrates the use of the waitForInterrupt()
 *	function in wiringPi. It listens to a button input on
 *	BCM_GPIO pin 17 (wiringPi pin 0)
 *
 *	The biggest issue with this method is that it really only works
 *	well in Sys mode.
 *
 *	Jan 2013: This way of doing things is sort of deprecated now, see
 *	the wiringPiISR() function instead and the isr.c test program here.
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
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
#include <stdlib.h>
#include <wiringPi.h>

// A 'key' which we can lock and unlock - values are 0 through 3
//	This is interpreted internally as a pthread_mutex by wiringPi
//	which is hiding some of that to make life simple.

#define	COUNT_KEY	0

// What BCM_GPIO input are we using?

#define	BUTTON_PIN	17

// Debounce time in mS

#define	DEBOUNCE_TIME	100


// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter = 0 ;


/*
 * waitForIt:
 *	This is a thread created using the wiringPi simplified threading
 *	mechanism. It will wait on an interrupt on the button and increment
 *	a counter.
 *********************************************************************************
 */

PI_THREAD (waitForIt)
{
  int state = 0 ;
  int debounceTime = 0 ;

  (void)piHiPri (10) ;	// Set this thread to be high priority

  for (;;)
  {
    if (waitForInterrupt (BUTTON_PIN, -1) > 0)	// Got it
    {
// Bouncing?

      if (millis () < debounceTime)
      {
	debounceTime = millis () + DEBOUNCE_TIME ;
	continue ;
      }

// We have a valid one

      state ^= 1 ;

      piLock (COUNT_KEY) ;
	++globalCounter ;
      piUnlock (COUNT_KEY) ;

// Wait for key to be released

      while (digitalRead (BUTTON_PIN) == LOW)
	delay (1) ;

      debounceTime = millis () + DEBOUNCE_TIME ;
    }
  }
}


/*
 * setup:
 *	Demo a crude but effective way to initialise the hardware
 *********************************************************************************
 */

void setup (void)
{

// Use the gpio program to initialise the hardware
//	(This is the crude, but effective)

  system ("gpio edge 17 falling") ;

// Setup wiringPi

  wiringPiSetupSys () ;

// Fire off our interrupt handler

  piThreadCreate (waitForIt) ;

}


/*
 * main
 *********************************************************************************
 */

int main (void)
{
  int lastCounter = 0 ;
  int myCounter   = 0 ;

  setup () ;

  for (;;)
  {
    printf ("Waiting ... ") ; fflush (stdout) ;

    while (myCounter == lastCounter)
    {
      piLock (COUNT_KEY) ;
	myCounter = globalCounter ;
      piUnlock (COUNT_KEY) ;
      delay (500) ;
    }

    printf (" Done. myCounter: %5d\n", myCounter) ;
    lastCounter = myCounter ;
  }

  return 0 ;
}
