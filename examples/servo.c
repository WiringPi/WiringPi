/*
 * servo.c:
 *	Test of the softServo code.
 *	Do not use this code - use the servoBlaster kernel module instead
 *
 * Copyright (c) 2012-2013 Gordon Henderson. <projects@drogon.net>
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
#include <errno.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <softServo.h>

// GPIO 1 (physical pin 12)
static const int cPin = 1;

int main()
{
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }

    softServoSetup(cPin, -1, -1, -1, -1, -1, -1, -1);

//    softServoWrite(1, -250);
//    delay(1000);
//    softServoWrite(1, 0);
//    delay(1000);
//    softServoWrite(1, 50);
//    delay(1000);
//    softServoWrite(1, 150);
//    delay(1000);
//    softServoWrite(1, 200);
//    delay(1000);
//    softServoWrite(1, 250);
//    delay(1000);
//    softServoWrite(1, 1250);
//    delay(1000);
    /*
     softServoWrite (1, 1000) ;  delay(1000);
     softServoWrite (2, 1100) ;  delay(1000);
     softServoWrite (3, 1200) ;  delay(1000);
     softServoWrite (4, 1300) ;  delay(1000);
     softServoWrite (5, 1400) ;  delay(1000);
     softServoWrite (6, 1500) ;  delay(1000);
     softServoWrite (7, 2200) ;  delay(1000);
     */

    int i = 1;
    int step = 0;
    const int cMin = -250;
    const int cMax = 1250;
    const int cStep = 100;

    printf("\n");
    printf("Home, Max, Home...\n");
    softServoWrite(cPin, cMax);
    softServoWrite(cPin, cMin);
    softServoWrite(cPin, cMax);
    delay(1000);

    printf("Stepping %d steps...\n", (cMax - cMin)/cStep);
    for (step = cMin; step < cMax; step += cStep, ++i)
    {
        printf("%d...", i); fflush(stdout);
        softServoWrite(cPin, step);
        delay(300);
    }
    printf("\n");
    printf("Returning to home position.\n");
    softServoWrite(cPin, cMin);
    delay(1000);

    const int cStep1 = 50;
    const int cRange = (cMax - cMin) / cStep1;
    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));
    const int cPositions = 20;
    int pos = cMin;
    int lastPos = pos;
    printf("Setting %d random positions...\n", cPositions);
    for (i = 1; i <= cPositions; ++i)
    {
        while (pos == lastPos)
        {
            pos = ((rand() % cRange) * cStep1) + cMin;
        }
        printf("%d...", i); fflush(stdout);
        softServoWrite(cPin, pos);
        lastPos = pos;
        delay(200);
    }

    printf("\n");
    printf("Returning to home position.\n");
    softServoWrite(cPin, cMin);
    delay(1000);

    return 0;
}
