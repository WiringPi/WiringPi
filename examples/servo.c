/*
 * servo.c:
 *	Test of the softServo code.
 *	Do not use this code - use the servoBlaster kernel module instead
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include <wiringPi.h>
#include <softServo.h>

// *****************************************************************************

// GPIO 12 (physical pin 32)
static const int cGpioPin = 12;

static const int cPwmFreq = 50; // Hz
static const int cPwmc=60;
static const int cPwmr=10000;

static const int cMin = 167;
static const int cMid = 416;
static const int cMax = 733;
static const int cStep = 10;

static void SignalHandler(int sig);

// *****************************************************************************

void init()
{
    printf("Init\n");
    pinMode(cGpioPin, PWM_OUTPUT);
    pwmToneWrite(cGpioPin, cPwmFreq);
    pwmSetMode(PWM_MODE_MS);

    pwmSetClock(cPwmc);
    pwmSetRange(cPwmr);
    pwmWrite(cGpioPin, cMid);
    delayMs(1000);
}

void reset()
{
    printf("Reset\n");
    pinMode(cGpioPin, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);

    pwmSetClock(cPwmc);
    pwmSetRange(cPwmr);
    pwmWrite(cGpioPin, cMid);
    delayMs(500);

    pinMode(cGpioPin, OUTPUT);
    delayMs(1000);
}

void rangeUp()
{
    int pos = 0;
    for (int i=cMin; i <= cMax; i +=cStep )
    {
        printf("  %d: ", i);
        pwmWrite(cGpioPin, i);
        delayMs(100);
        pos = pwmRead(cGpioPin);
        printf("%d\n", pos);
    }
}

void rangeDn()
{
    int pos = 0;
    for (int i=cMax; i >= cMin; i -=cStep )
    {
        printf("  %d: ", i);
        pwmWrite(cGpioPin, i);
        delayMs(100);
        pos = pwmRead(cGpioPin);
        printf("%d\n", pos);
    }
}

// *****************************************************************************

int main(int argc, char *argv[])
{
    printf("\n%s: [%s] %s @ %d\n\n", basename(argv[0]), __FILE__, __FUNCTION__, __LINE__);

    signal(SIGABRT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    signal(SIGINT,  SignalHandler);

    if (wiringPiSetupGpio() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }

    init();

    printf("Setting to middle position.\n");
    pwmWrite(cGpioPin, cMid);
    delayMs(1000);

    printf("Moving up\n");
    rangeUp();
    delayMs(1000);

    printf("Moving down\n");
    rangeDn();
    delayMs(1000);


    printf("Max\n");
    pwmWrite(cGpioPin, cMax);
    delayMs(1000);
    printf("Min\n");
    pwmWrite(cGpioPin, cMin);
    delayMs(1000);
    printf("Max\n");
    pwmWrite(cGpioPin, cMax);
    delayMs(1000);
    printf("Min\n");
    pwmWrite(cGpioPin, cMin);
    delayMs(1000);

    printf("Returning to middle position.\n");
    pwmWrite(cGpioPin, cMid);
    delayMs(1000);

#if 0
    delayMs(1000);

    const int cStep1 = 5;
    const int cRange = (cMax - cMin) / cStep1;
    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));
    const int cPositions = 20;
    int pos = cMin;
    int lastPos = pos;
    printf("Setting %d random positions...\n", cPositions);
    for (int i = 1; i <= cPositions; ++i)
    {
        while (pos == lastPos)
        {
            pos = ((rand() % cRange) * cStep1) + cMin;
        }
        printf("%d...", i); fflush(stdout);
        pwmWrite(cGpioPin, pos);
        lastPos = pos;
        delayMs(300);
    }

    printf("Returning to middle position.\n");
    pwmWrite(cGpioPin, cMid);
    delayMs(1000);
#endif

    return 0;
}

// *****************************************************************************

void SignalHandler(int sig)
{
    fprintf(stderr, "\nSignal %d caught by %d\n", sig, getpid());
    reset();
    exit(2);
}

// *****************************************************************************
