/*
 * piFace.c:
 *	Simple test for the PiFace interface board.
 *
 *	Read the buttons and output the same to the LEDs
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

int outputs [4] = { 0,0,0,0 } ;

void scanButton (int button)
{
  if (digitalRead (button) == LOW)
  {
    outputs [button] ^= 1 ;
    digitalWrite (button, outputs [button]) ;
  }

  while (digitalRead (button) == LOW)
    delay (1) ;
}


int main (void)
{
  int pin, button ;

  printf ("Raspberry Pi wiringPiFace test program\n") ;

  if (wiringPiSetupPiFace () == -1)
    exit (1) ;

// Enable internal pull-ups

  for (pin = 0 ; pin < 8 ; ++pin)
    pullUpDnControl (pin, PUD_UP) ;


  for (;;)
  {
    for (button = 0 ; button < 4 ; ++button)
      scanButton (button) ;
    delay (1) ;
  }

  return 0 ;
}
