/*
 * lowPower.c:
 *	Check the Pi's LOW-Power signal.
 *
 *	This is a demonstration program that could be turned into some sort
 *	of logger via e.g. syslog - however it's also probably something
 *	that might be better handled by a future kernel - who knows.
 *
 * Copyright (c) 2014 Gordon Henderson.
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
#include <time.h>
#include <wiringPi.h>


#define	LOW_POWER	35

/*
 * lowPower:
 *	This is an ISR that waits for the low-power signal going low and
 *	prints the result.
 *********************************************************************************
 */

void lowPower (void)
{
  time_t t ;

  time (&t) ;
  printf ("%s: LOW POWER DETECTED\n", ctime (&t)) ;
}


/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main (void)
{
  wiringPiSetupGpio () ;	// GPIO mode as it's an internal pin

  wiringPiISR (LOW_POWER, INT_EDGE_FALLING, &lowPower) ;

  for (;;)
    delay (1000) ;

  return 0 ;
}
