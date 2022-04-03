/*
 * buttons4.c:
 *	Multiple button handling with debounce.
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
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <wiringPi.h>

//**********************************************************************************************************************

// Define the GPIOs (BCM) that buttons are attached to.
// Button1, Button2, ... , ButtonN-1
static const int buttonGpios[] = { 26, 16, 20, 21 };

// Button flags: 1 = button was pressed.
// @NOTE The size of this array must match the number of GPIOs specified in buttonGpios above
static int buttons[4];

static const int nButtons = (sizeof (buttonGpios) / sizeof (int));

// Debounce time in mS
#define DEBOUNCE_TIME 50

static int terminate_process = 0;

//**********************************************************************************************************************

static void Signal_handler(int sig);

//**********************************************************************************************************************

/* debounce
 *  Thread to monitor button GPIOs and debounce them.
 */
static void *debounce (UNUSED void *arg)
{
    int masks[nButtons];
    int button_times[nButtons];

    // Set the GPIOs as inputs
    // Also set up the gpio bank masks for each button
    for (int i = 0; i < nButtons; ++i)
    {
        pinMode (buttonGpios[i], INPUT);
        pullUpDnControl (buttonGpios[i], PUD_UP);

        // Set up GPIO masks for the GPIO bank
        masks[i] = (1 << buttonGpios[i]);

        // Clear button flags and counters
        buttons[i] = 0;
        button_times[i] = 0;
    }

    // --------------------------------
    // GPIO monitor & debounce loop
    uint32_t bank0 = 0;
    uint32_t prev_button_states[nButtons];

    while (!terminate_process)
    {
        usleep(5 * 1000);

        bank0 = digitalReadBank(0);

        for (int i=0; i < nButtons; ++i)
        {
            int buttonState = (bank0 & masks[i]);

            // If button changed state, reset its timer.
            if (buttonState != prev_button_states[i])
            {
                button_times[i] = millis();
            }

            if ((millis() - button_times[i]) >= DEBOUNCE_TIME)
            {
                // Pressed button = LOW = 0
                // Set the state to 1 if pressed, 0 if released.
                buttons[i] = !buttonState;
            }

            prev_button_states[i] = buttonState;
        }
    } // end while

    // --------------------------------

    printf("Debounce done.\n");

    return (void *)NULL;
}

//**********************************************************************************************************************

static void Button_Wait (int pin)
{
    printf(" %-2d  %-2d  %-2d  %-2d\n", buttons[0], buttons[1], buttons[2], buttons[3]);
}

//**********************************************************************************************************************

int main (int argc, char *argv[])
{
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

    // ----------------------------------------------------

    // Set up for BCM pin numbering
    wiringPiSetupGpio ();

    // ----------------------------------------------------
    // Set up the button debounce thread

    pthread_t debounceThreadId;
    pthread_create (&debounceThreadId, NULL, debounce, NULL) ;

    // ----------------------------------------------------

    // Set up a callback for when any of these buttons is pressed/released
    // The ISR will display the state of all buttons.
    if (wiringPiISRmulti ((int *)buttonGpios, nButtons, INT_EDGE_RISING, &Button_Wait) < 0)
    {
        fprintf (stderr, "%s: Unable to setup ISR: %s\n", argv[0], strerror(errno));
        return EXIT_FAILURE;
    }

    // ----------------------------------------------------

    printf("Buttons:\n");
    printf(" 1   2   3   4\n");
    printf("--- --- --- ---\n");
    while (!terminate_process)
    {
        delayMs(100);
    }

    // ----------------------------------------------------

    pthread_join(debounceThreadId, NULL);

    printf("%s done.\n", argv[0]);

    return EXIT_SUCCESS;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals from QNX
 * This function is called when the SIGTERM signal is raised by QNX
 */
static void Signal_handler(int sig)
{
  printf("Received signal %d\n", sig);

  // Signal process to exit.
  terminate_process = 1;
}

//**********************************************************************************************************************
