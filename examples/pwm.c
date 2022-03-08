/*
 * pwm.c:
 *	This tests the hardware PWM channel.
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

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

//**********************************************************************************************************************

static int terminate_process = 0;

static void Signal_handler(int sig);

//**********************************************************************************************************************

int main(void)
{
    int bright;

    printf("Raspberry Pi wiringPi PWM test program\n");

    if (wiringPiSetup() == -1)
    {
        printf("ERROR: wiringPi setup failed\n");
        return EXIT_FAILURE;
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

    pinMode(1, PWM_OUTPUT);

    printf("Going into +/- loop...\n");
    while (!terminate_process)
    {
        for (bright = 0; !terminate_process && bright < 1024; ++bright)
        {
            printf("+"); fflush(stdout);
            pwmWrite(1, bright);
            //delay (1) ;
            usleep(5 * 1000);
        }

        for (bright = 1023; !terminate_process && bright >= 0; --bright)
        {
            printf("-"); fflush(stdout);
            pwmWrite(1, bright);
            //delay (1) ;
            usleep(5 * 1000);
        }
    }

    return 0;
}

//**********************************************************************************************************************

/**
 * Intercepts and handles signals
 * This function is called when the SIGTERM signal is raised
 */
void Signal_handler(int sig)
{
    printf("Received signal %d\n", sig);

    // Signal process to exit.
    terminate_process = 1;
}

//**********************************************************************************************************************
