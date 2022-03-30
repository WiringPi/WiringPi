/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method
 *
 *	How to test:
 *	  Use the SoC's pull-up and pull down resistors that are avalable
 *	on input pins. So compile & run this program (via sudo), then
 *	in another terminal:
 *		gpio mode 0 up
 *		gpio mode 0 down
 *	at which point it should trigger an interrupt. Toggle the pin
 *	up/down to generate more interrupts to test.
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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
#include <pthread.h>

//**********************************************************************************************************************

// What GPIO input are we using?
// This is a wiringPi pin number
#define	BUTTON_PIN 3

// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
#define LED 2

//**********************************************************************************************************************

// Interrupt state
static volatile int isr_state = 0;

static int terminate_process = 0;

static pthread_mutex_t smb_mutex = PTHREAD_MUTEX_INITIALIZER;

//**********************************************************************************************************************

static void Signal_handler(int sig);

//**********************************************************************************************************************

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt(int pin) {
  // Only report button release on actual button release.
  // This interrupt sometimes fires for the wrong edge!

  if (!isr_state) {
    isr_state = 1;
    pthread_mutex_unlock( &smb_mutex );
  }
}

/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main(void) {
  int button_state = 0;
  int led_state = 0;

  if (wiringPiSetup() < 0) {
    fprintf(stderr, "Unable to setup wiringPi: %s\n", strerror(errno));
    return 1;
  }

  pinMode(LED, OUTPUT);

  if (wiringPiISR(BUTTON_PIN, INT_EDGE_BOTH, &myInterrupt) < 0) {
    fprintf(stderr, "Unable to setup ISR: %s\n", strerror(errno));
    return 1;
  }

  // Set the handler for SIGTERM (15)
  signal(SIGTERM, Signal_handler);
  signal(SIGHUP,  Signal_handler);
  signal(SIGINT,  Signal_handler);
  signal(SIGQUIT, Signal_handler);
  signal(SIGTRAP, Signal_handler);
  signal(SIGABRT, Signal_handler);
  signal(SIGALRM, Signal_handler);
  signal(SIGUSR1, Signal_handler);
  signal(SIGUSR2, Signal_handler);

  pthread_mutex_lock( &smb_mutex );
  while (!terminate_process) {
    printf("Waiting ... ");
    fflush(stdout);

    pthread_mutex_lock( &smb_mutex );
    delayMs(50);
    isr_state = 0;

    button_state = digitalRead(BUTTON_PIN);
    printf("  Button state: %d\n", button_state);
    if (button_state == 1) {
      digitalWrite (LED, led_state);
      led_state = !led_state;
    }
  }

  return 0;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals from QNX
 * This function is called when the SIGTERM signal is raised by QNX
 */
void Signal_handler(int sig) {
  printf("Received signal %d\n", sig);

  // Signal process to exit.
  terminate_process = 1;

  pthread_mutex_unlock( &smb_mutex );
}

//**********************************************************************************************************************
