/*
 * buttons.c:
 *	Button polling, with debounce.
 *  Note: This handles only one button at a time.
 *
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/nuncio-bitis/WiringPi
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published
 *by the Free Software Foundation, either version 3 of the License, or (at your
 *option) any later version.
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
#include <wiringPi.h>

//**********************************************************************************************************************

// GPIOs (BCM) that buttons are attached to:
static const int buttons[] = { 26, 16, 20, 21 };
static const int nButtons = (sizeof (buttons) / sizeof (int));

static int terminate_process = 0;
static void Signal_handler(int sig);

//**********************************************************************************************************************

// scanButton:
// See if a button is pushed, if so, then flip that LED and
// wait for the button to be let-go
void scanButton (int button)
{
    if (digitalRead (button) == HIGH) // Low is pushed
    {
        return;
    }

    int start = micros();
    printf ("Button %d pressed\n", button);

    // Wait for release
    while (digitalRead (button) == LOW)
    {
        delayMs(1);
    }
    int dur = micros() - start;

    printf ("Button %d released; %d uS\n", button, dur);
}

//**********************************************************************************************************************

int main (void)
{
    int i;

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

    printf ("Raspberry Pi Button Test\n");

    wiringPiSetupGpio ();

    // Setup the inputs
    for (i = 0; i < nButtons; ++i)
    {
        pinMode (buttons[i], INPUT);
        pullUpDnControl (buttons[i], PUD_UP);
    }

    while (!terminate_process)
    {
        for (i = 0; i < nButtons; ++i)
        {
            scanButton (buttons[i]);
        }
        delayMs(1);
    }

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
}

//**********************************************************************************************************************
