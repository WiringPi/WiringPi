/*
 * buttons2.c:
 *	Button polling, with debounce.
 *  Note: This tries to handle all buttons simultaneously.
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

    int buttonCounts[] = {0, 0, 0, 0};
    int buttonVal[] = {0, 0, 0, 0};
    const int threshold = 10;
    while (!terminate_process)
    {

        for (i = 0; i < nButtons; ++i)
        {
            // If released, look for press
            if (buttonVal[i] == 0)
            {
                if (digitalRead (buttons[i]) == LOW) // Low is pushed
                {
                    if (buttonCounts[i] < threshold)
                    {
                        buttonCounts[i]++;
                    }
                    else
                    {
                        buttonVal[i] = 1; // declare pushed
                        printf ("Button %d pressed\n", buttons[i]);
                    }
                }
            }

            delayMs(1);

            // If pushed, look for release
            if (buttonVal[i] == 1)
            {
                if (digitalRead (buttons[i]) == HIGH)
                {
                    if (buttonCounts[i] > 0)
                    {
                        buttonCounts[i]--;
                    }
                    else
                    {
                        buttonVal[i] = 0; // declare released
                        printf ("Button %d released\n", buttons[i]);
                    }
                }
            }

        }

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
