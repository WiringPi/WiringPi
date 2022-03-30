/*
 * wfi.c:
 *  Wait for Interrupt test program
 *
 *  This program demonstrates the use of the waitForInterrupt()
 *  function in wiringPi. It listens to a button input on
 *  BCM_GPIO pin 17 (wiringPi pin 0)
 *
 *  The biggest issue with this method is that it really only works
 *  well in Sys mode.
 *
 *  Jan 2013: This way of doing things is sort of deprecated now, see
 *  the wiringPiISR() function instead and the isr.c test program here.
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *  https://github.com/WiringPi/WiringPi/
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
#include <signal.h>
#include <pthread.h>

#include <wiringPi.h>

static int terminate_process = 0;
static void Signal_handler(int sig);

// A 'key' which we can lock and unlock - values are 0 through 3
// This is interpreted internally as a pthread_mutex by wiringPi
// which is hiding some of that to make life simple.

#define COUNT_KEY 0

// What BCM_GPIO input are we using?

#define BUTTON_PIN 26

// Debounce time in mS

#define DEBOUNCE_TIME 100


// globalCounter:
// Global variable to count interrupts
// Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter = 0;


/*
 * waitForIt:
 *  This is a thread created using the wiringPi simplified threading
 *  mechanism. It will wait on an interrupt on the button and increment
 *  a counter.
 *********************************************************************************
 */

PI_THREAD (waitForIt)
{
  int state = 0;
  int debounceTime = 0;

  (void)piHiPri (10); // Set this thread to be high priority

  (void)digitalRead (BUTTON_PIN); // pre-read pin to avoid false trigger

  while (!terminate_process)
  {
    if (waitForInterrupt (BUTTON_PIN, -1) > 0) // Got it
    {
      // Bouncing?
      if (millis() < debounceTime)
      {
        debounceTime = millis() + DEBOUNCE_TIME;
        continue;
      }

      // We have a valid one
      state ^= 1;

      piLock (COUNT_KEY);
      ++globalCounter;
      piUnlock (COUNT_KEY);

      // Wait for key to be released
      while (digitalRead (BUTTON_PIN) == LOW)
      {
        delayMs (10);
      }

      debounceTime = millis() + DEBOUNCE_TIME;
    }
  }

  return (void *) NULL;
}


/*
 * main
 *********************************************************************************
 */

int main (void)
{
  int lastCounter = 0;
  int myCounter   = 0;

  // ----------------------------------
  // Demo a crude but effective way to initialise the hardware

  // Use the gpio program to initialise the hardware
  //  (This is the crude, but effective)
  system ("gpio edge 26 falling");

  // Setup wiringPi
  wiringPiSetupSys();

  // Fire off our interrupt handler
  int tid = piThreadCreate (waitForIt);

  // ----------------------------------

  // Set the handler for SIGTERM (15)
  signal(SIGTERM, Signal_handler);
  signal(SIGINT,  Signal_handler);
  signal(SIGQUIT, Signal_handler);
  signal(SIGTRAP, Signal_handler);
  signal(SIGABRT, Signal_handler);
  signal(SIGALRM, Signal_handler);
  signal(SIGUSR1, Signal_handler);
  signal(SIGUSR2, Signal_handler);

  while (myCounter < 25)
  {
    printf ("Waiting ... ");
    fflush (stdout);

    while (myCounter == lastCounter)
    {
      piLock (COUNT_KEY);
      myCounter = globalCounter;
      piUnlock (COUNT_KEY);
      delayMs(75);
    }
    if (terminate_process) break;

    printf (" Done. myCounter: %5d\n", myCounter);
    lastCounter = myCounter;
  }
  pthread_join(tid, NULL);

  printf ("Done.\n");

  return EXIT_SUCCESS;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals from QNX
 * This function is called when the SIGTERM signal is raised by QNX
 */
void Signal_handler(int sig)
{
  printf("Received signal %d\n", sig);

  // Signal process to exit.
  terminate_process = 1;

  globalCounter++;
}

//**********************************************************************************************************************
