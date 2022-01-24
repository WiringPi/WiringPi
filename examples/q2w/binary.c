/*
 * binary.c:
 *      Using the Quick 2 wire 16-bit GPIO expansion board to output
 *	a binary counter.
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
#include <mcp23017.h>

#define	Q2W_BASE	100

int main (void)
{
  int i, bit ;

// Enable the on-goard GPIO

  wiringPiSetup () ;

// Add in the mcp23017 on the q2w board

  mcp23017Setup (Q2W_BASE, 0x20) ;

  printf ("Raspberry Pi - quite2Wire MCP23017 Test\n") ;

// On-board button Input:

  pinMode (0, INPUT) ;

// First 10 pins on q2w board as outputs:

  for (i = 0 ; i < 10 ; ++i)
    pinMode (Q2W_BASE + i, OUTPUT) ;

// Last pin as an input with the internal pull-up enabled

  pinMode         (Q2W_BASE + 15, INPUT) ;
  pullUpDnControl (Q2W_BASE + 15, PUD_UP) ;

// Loop, outputting a binary number,
//	Go faster with the button, or stop if the
//	on-board button is pushed

  for (;;)
  {
    for (i = 0 ; i < 1024 ; ++i)
    {
      for (bit = 0 ; bit < 10 ; ++bit)
        digitalWrite (Q2W_BASE + bit, i & (1 << bit)) ;

      while (digitalRead (0) == HIGH)		// While pushed
	delay (1) ;

      if (digitalRead (Q2W_BASE + 15) == HIGH)	// Not Pushed
	delay (100) ;
    }
  }
  return 0 ;
}
