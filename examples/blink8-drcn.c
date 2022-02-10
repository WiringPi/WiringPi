/*
 * blink8-drcn.c:
 *	Simple sequence over the first 8 GPIO pins - LEDs
 *	Aimed at the Ladder board, but it's fairly generic.
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
#include <drcNet.h>

int main (void)
{
  int i, led ;

  printf ("Raspberry Pi - 8-LED Sequencer\n") ;
  printf ("==============================\n") ;
  printf ("\n") ;
  printf ("Connect LEDs to the first 8 GPIO pins and watch ...\n") ;

  int pinBase = 100 ;

//  wiringPiSetup () ;
  drcSetupNet (pinBase, 100, "192.168.254.21", "6124", "123456") ;

  for (i = 0 ; i < 8 ; ++i)
    pinMode (i + pinBase, OUTPUT) ;

  for (;;)
  {
    for (led = 0 ; led < 8 ; ++led)
    {
      digitalWrite (led + pinBase, 1) ;
      delay (10) ;
    }

    for (led = 0 ; led < 8 ; ++led)
    {
      digitalWrite (led + pinBase, 0) ;
      delay (10) ;
    }
  }
}
