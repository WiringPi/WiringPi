/*
 * bright.c:
 *	Vary the Q2W LED brightness with the analog card
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
#include <wiringPi.h>
#include <pcf8591.h>

#define	LED		  1
#define Q2W_ABASE       120

int main (void)
{
  int value ;

// Enable the on-goard GPIO

  wiringPiSetup () ;

// Add in the pcf8591 on the q2w board

  pcf8591Setup (Q2W_ABASE, 0x48) ;

  printf ("Raspberry Pi - Quick2Wire Analog Test\n") ;

// Setup the LED

  pinMode  (LED, PWM_OUTPUT) ;
  pwmWrite (LED, 0) ;

  for (;;)
  {
    value = analogRead  (Q2W_ABASE + 0) ;
    pwmWrite (LED, value * 4) ;
    delay (10) ;
  }

  return 0 ;
}
