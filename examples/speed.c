/*
 * speed.c:
 *	Simple program to measure the speed of the various GPIO
 *	access mechanisms.
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

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define	FAST_COUNT	10000000
#define	SLOW_COUNT	 1000000


int main (void)
{
  int i ;
  uint32_t start, end, count, sum, perSec ;

  printf ("Raspberry Pi wiringPi speed test program\n") ;

// Start the standard way

  if (wiringPiSetup () == -1)
    exit (1) ;

  printf ("Native wiringPi method: (%8d iterations)\n", FAST_COUNT) ;

  pinMode (0, OUTPUT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < FAST_COUNT ; ++count)
      digitalWrite (0, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (0, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)FAST_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;


  printf ("Native GPIO method: (%8d iterations)\n", FAST_COUNT) ;

  if (wiringPiSetupGpio () == -1)
    exit (1) ;

  pinMode (17, OUTPUT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < 10000000 ; ++count)
      digitalWrite (17, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (17, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)FAST_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;


// Switch to SYS mode:

  if (wiringPiSetupSys () == -1)
    exit (1) ;

  printf ("/sys/class/gpio method: (%8d iterations)\n", SLOW_COUNT) ;

  sum = 0 ;
  for (i = 0 ; i < 3 ; ++i)
  {
    printf ("  Pass: %d: ", i) ;
    fflush (stdout) ;

    start = millis () ;
    for (count = 0 ; count < SLOW_COUNT ; ++count)
      digitalWrite (17, 1) ;
    end = millis () ;
    printf (" %8dmS\n", end - start) ;
    sum += (end - start) ;
  }
  digitalWrite (17, 0) ;
  printf ("   Average: %8dmS", sum / 3) ;
  perSec = (int)(double)SLOW_COUNT / (double)((double)sum / 3.0) * 1000.0 ;
  printf (": %6d/sec\n", perSec) ;

  return 0 ;
}
