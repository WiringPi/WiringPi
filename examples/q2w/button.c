/*
 * button.c:
 *	Simple button test for the Quick2Wire interface board.
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
#include <wiringPi.h>

#define	BUTTON	0
#define	LED1	1
#define	LED2	7

int main (void)
{

// Enable the on-goard GPIO

  wiringPiSetup () ;

  printf ("Raspberry Pi - Quick2Wire Mainboard Button & LED Test\n") ;

  pinMode (BUTTON, INPUT) ;
  pinMode (LED1, OUTPUT) ;
  pinMode (LED2, OUTPUT) ;

  digitalWrite (LED1, HIGH) ;		// On-board LED on
  digitalWrite (LED2, LOW) ;		// 2nd LED off

  for (;;)
  {
    if (digitalRead (BUTTON) == HIGH)	// Swap LED states
    {
      digitalWrite (LED1, LOW) ;
      digitalWrite (LED2, HIGH) ;
      while (digitalRead (BUTTON) == HIGH)
	delay (1) ;
      digitalWrite (LED1, HIGH) ;
      digitalWrite (LED2, LOW) ;
    }
    delay (1) ;
  }

  return 0 ;
}
