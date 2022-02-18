/*
 * speed.c:
 *	Simple program to measure the speed of the various GPIO
 *	access mechanisms.
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

#define	FAST_COUNT	10000000
#define	SLOW_COUNT	 1000000
#define	PASSES		       5

void speedTest (int pin, int maxCount)
{
  int count, sum, perSec, i ;
  unsigned int start, end ;

  sum = 0 ;

  for (i = 0 ; i < PASSES ; ++i)
  {
    start = millis () ;
    for (count = 0 ; count < maxCount ; ++count)
      digitalWrite (pin, 1) ;
    end = millis () ;
    printf (" %6d", end - start) ;
    fflush (stdout) ;
    sum += (end - start) ;
  }

  digitalWrite (pin, 0) ;
  printf (". Av: %6dmS", sum / PASSES) ;
  perSec = (int)(double)maxCount / (double)((double)sum / (double)PASSES) * 1000.0 ;
  printf (": %7d/sec\n", perSec) ;
}


int main (void)
{
  printf ("Raspberry Pi wiringPi GPIO speed test program\n") ;
  printf ("=============================================\n") ;

// Start the standard way

  printf ("\nNative wiringPi method: (%8d iterations)\n", FAST_COUNT) ;
  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;
  speedTest (0, FAST_COUNT) ;

// GPIO

  printf ("\nNative GPIO method: (%8d iterations)\n", FAST_COUNT) ;
  wiringPiSetupGpio () ;
  pinMode (17, OUTPUT) ;
  speedTest (17, FAST_COUNT) ;

// Phys

  printf ("\nPhysical pin GPIO method: (%8d iterations)\n", FAST_COUNT) ;
  wiringPiSetupPhys () ;
  pinMode (11, OUTPUT) ;
  speedTest (11, FAST_COUNT) ;

// Switch to SYS mode:

  system ("/usr/local/bin/gpio export 17 out") ;
  printf ("\n/sys/class/gpio method: (%8d iterations)\n", SLOW_COUNT) ;
  wiringPiSetupSys () ;
  speedTest (17, SLOW_COUNT) ;

  return 0 ;
}
