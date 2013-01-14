/*
 * wiringPiISR.c:
 *	Simplified Interrupt Service Routine handling
 *	Copyright (c) 2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "wiringPi.h"



static void (*isrFunctions [64])(void) ;
static int    isrFds       [64] ;

/*
 * interruptHandler:
 *	This is a thread and gets started to wait for the interrupt we're
 *	hoping to catch. It will call the user-function when the interrupt
 *	fires.
 *********************************************************************************
 */

static void *interruptHandler (void *arg)
{
  int pin = *(int *)arg ;

  (void)piHiPri (55) ;

  for (;;)
  {
    if (waitForInterrupt (pin, -1) > 0)
      isrFunctions [pin] () ;
  }

  return NULL ;
}

/*
 * wiringPiISR:
 *	Take the details and create an interrupt handler that will do a call-
 *	back to the user supplied function.
 *********************************************************************************
 */

int wiringPiISR (int pin, int mode, void (*function)(void))
{
  pthread_t threadId ;
  char command [64] ;

  pin &= 63 ;

  if (wiringPiMode == WPI_MODE_UNINITIALISED)
  {
    fprintf (stderr, "wiringPiISR: wiringPi has not been initialised. Unable to continue.\n") ;
    exit (EXIT_FAILURE) ;
  }
  else if (wiringPiMode == WPI_MODE_PINS)
    pin = pinToGpio [pin] ;


  isrFunctions [pin] = function ;

// Now export the pin and set the right edge

  if (mode != INT_EDGE_SETUP)
  {
    /**/ if (mode == INT_EDGE_FALLING)
      modes = "falling" ;
    else if (mode == INT_EDGE_RISING)
      modes = "rising" ;
    else
      modes = "both" ;

    sprintf (command, "/usr/local/bin/gpio edge %d %s", pin, modes) ;
    system (command) ;
  }

  sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
  if ((isrFds [pin] = open (fName, O_RDWR)) < 0)
    return -1 ;

  {
    fprintf ("std

  pthread_create (&threadId, NULL, interruptHandler, &pin) ;
}


