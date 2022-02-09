/*
 * softPwm.c:
 *	Test of the software PWM driver. Needs 8 LEDs connected
 *	to the Pi - e.g. Ladder board.
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
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softPwm.h>

#define RANGE		100
#define	NUM_LEDS	  8

int ledMap [NUM_LEDS] = { 0, 1, 2, 3, 4, 5, 6, 7 } ;

int values [NUM_LEDS] = { 0, 25, 50, 75, 100, 75, 50, 25 } ;

int main ()
{
  int i, j ;
  char buf [80] ;

  wiringPiSetup ()  ;

  for (i = 0 ; i < NUM_LEDS ; ++i)
  {
    softPwmCreate (ledMap [i], 0, RANGE) ;
    printf ("%3d, %3d, %3d\n", i, ledMap [i], values [i]) ;
  }

  fgets (buf, 80, stdin) ;

// Bring all up one by one:

  for (i = 0 ; i < NUM_LEDS ; ++i)
    for (j = 0 ; j <= 100 ; ++j)
    {
      softPwmWrite (ledMap [i], j) ;
      delay (10) ;
    }

  fgets (buf, 80, stdin) ;

// All Down

  for (i = 100 ; i > 0 ; --i)
  {
    for (j = 0 ; j < NUM_LEDS ; ++j)
      softPwmWrite (ledMap [j], i) ;
    delay (10) ;
  }

  fgets (buf, 80, stdin) ;

  for (;;)
  {
    for (i = 0 ; i < NUM_LEDS ; ++i)
      softPwmWrite (ledMap [i], values [i]) ;

    delay (50) ;

    i = values [0] ;
    for (j = 0 ; j < NUM_LEDS - 1 ; ++j)
      values [j] = values [j + 1] ;
    values [NUM_LEDS - 1] = i ;
  }
}
