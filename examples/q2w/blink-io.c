/*
 * blink-io.c:
 *	Simple "blink" test for the Quick2Wire 16-pin IO board.
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

#define	LED		1
#define	Q2W_BASE	100

int main (void)
{

// Enable the on-goard GPIO

  wiringPiSetup () ;

// Add in the mcp23017 on the q2w board

  mcp23017Setup (Q2W_BASE, 0x20) ;

  printf ("Raspberry Pi - Quick2Wire MCP23017 Blink Test\n") ;

// Blink the on-board LED as well as one on the mcp23017

  pinMode (LED, OUTPUT) ;
  pinMode (Q2W_BASE + 0, OUTPUT) ;

  for (;;)
  {
    digitalWrite (LED,          HIGH) ;
    digitalWrite (Q2W_BASE + 0, HIGH) ;
    delay (500) ;
    digitalWrite (LED,          LOW) ;
    digitalWrite (Q2W_BASE + 0, LOW) ;
    delay (500) ;
  }

  return 0 ;
}
