/*
 * isr-osc.c:
 *	Wait for Interrupt test program - ISR method - interrupt oscillator
 *
 *	How to test:
 *
 *	IMPORTANT: To run this test we connect 2 GPIO pins together, but
 *	before we do that YOU must make sure that they are both set up
 *	the right way. If they are set to outputs and one is high and one low,
 *	then you connect the wire, you'll create a short and that won't be good.
 *
 *	Before making the connection, type:
 *		gpio mode 0 output
 *		gpio write 0 0
 *		gpio mode 1 input
 *	then you can connect them together.
 *
 *	Run the program, then:
 *		gpio write 0 1
 *		gpio write 0 0
 *
 *	at which point it will trigger an interrupt and the program will
 *	then do the up/down toggling for itself and run at full speed, and
 *	it will report the number of interrupts recieved every second.
 *
 *	Copyright (c) 2013 Gordon Henderson.
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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <signal.h>


// What GPIO input are we using?
//	This is a wiringPi pin number

#define	OUT_PIN		2
#define	IN_PIN		3

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.
static volatile int globalCounter = 0;

static int terminate_process = 0;

//**********************************************************************************************************************

static void Signal_handler(int sig);

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt (int pin)
{
  digitalWrite (OUT_PIN, 1) ;
  ++globalCounter;
  printf ("    %d\n", globalCounter);
  digitalWrite (OUT_PIN, 0) ;
}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
  int myCounter   = 0;

  if (wiringPiSetup () < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
    return 1;
  }

  pinMode (OUT_PIN, OUTPUT);
  pinMode (IN_PIN,  INPUT);

  if (wiringPiISR (IN_PIN, INT_EDGE_FALLING, &myInterrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
    return 1;
  }

  // Set the handler for SIGTERM (15)
  signal(SIGTERM, Signal_handler);
  signal(SIGHUP, Signal_handler);
  signal(SIGINT, Signal_handler);
  signal(SIGQUIT, Signal_handler);
  signal(SIGTRAP, Signal_handler);
  signal(SIGABRT, Signal_handler);
  signal(SIGALRM, Signal_handler);
  signal(SIGUSR1, Signal_handler);
  signal(SIGUSR2, Signal_handler);

  while (!terminate_process)
  {
    printf ("Waiting ...\n");
    fflush (stdout);

    while (!terminate_process && (myCounter == globalCounter))
    {
      delayMs(250);
    }

    printf ("Done. counter: %2d: %2d\n", globalCounter, globalCounter - myCounter);

    digitalWrite (OUT_PIN, 1);
    delayMs(500);
    digitalWrite (OUT_PIN, 0);

    myCounter   = globalCounter;
  }

  return 0;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals
 * This function is called when the SIGTERM signal is raised
 */
void Signal_handler(int sig) {
  printf("Received signal %d\n", sig);

  // Signal process to exit.
  terminate_process = 1;
}

//**********************************************************************************************************************
