/*
 * wfi.c:
 *	Wait for Interrupt test program
 *
 * Copyright (c) 2012 Gordon Henderson.
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
//	GPIO 0 is one of the I2C pins with an on-board pull-up

#define	BUTTON_PIN	0

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
  digitalWrite (18, 1) ;

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

      digitalWrite (17, state) ;
      state ^= 1 ;

      piLock (COUNT_KEY) ;
	++globalCounter ;
      piUnlock (COUNT_KEY) ;

// Wait for key to be released

      while (digitalRead (0) == LOW)
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
//	(This is the crude, but effective bit)

  system ("gpio   edge  0 falling") ;
  system ("gpio export 17 out") ;
  system ("gpio export 18 out") ;

// Setup wiringPi

  wiringPiSetupSys () ;

// Fire off our interrupt handler

  piThreadCreate   (waitForIt) ;

  digitalWrite (17, 0) ;
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
      delay (5000) ;
    }

    printf (" Done. myCounter: %5d\n", myCounter) ;
    lastCounter = myCounter ;
  }

  return 0 ;
}
