/*
 * motor.c:
 *	Use the PiFace board to demonstrate an H bridge
 *	circuit via the 2 relays.
 *	Then add on an external transsitor to help with PWM.
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
#include <piFace.h>
#include <softPwm.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int outputs [2] = { 0,0 } ;

#define	PIFACE_BASE	200
#define PWM_OUT_PIN	204
#define	PWM_UP		202
#define	PWM_DOWN	203

void scanButton (int button)
{
  if (digitalRead (PIFACE_BASE + button) == LOW)
  {
    outputs [button] ^= 1 ;
    digitalWrite (PIFACE_BASE + button, outputs [button]) ;
    printf ("Button %d pushed - output now: %s\n",
		button, (outputs [button] == 0) ? "Off" : "On") ;
  }

  while (digitalRead (PIFACE_BASE + button) == LOW)
    delay (1) ;
}


int main (void)
{
  int pin, button ;
  int pwmValue = 0 ;

  printf ("Raspberry Pi PiFace - Motor control\n") ;
  printf ("==================================\n") ;
  printf ("\n") ;
  printf (
"This program is designed to be used with a motor connected to the relays\n"
"in an H-Bridge type configuration with optional speeed control via PWM.\n"
"\n"
"Use the leftmost buttons to turn each relay on and off, and the rigthmost\n"
"buttons to increase ot decrease the PWM output on the control pin (pin\n"
"4)\n\n") ;

  wiringPiSetup () ;
  piFaceSetup (PIFACE_BASE) ;
  softPwmCreate (PWM_OUT_PIN, 100, 100) ;

// Enable internal pull-ups & start with all off

  for (pin = 0 ; pin < 8 ; ++pin)
  {
    pullUpDnControl (PIFACE_BASE + pin, PUD_UP) ;
    digitalWrite    (PIFACE_BASE + pin, 0) ;
  }

  for (;;)
  {
    for (button = 0 ; button < 2 ; ++button)
      scanButton (button) ;

    if (digitalRead (PWM_UP) == LOW)
    {
      pwmValue += 10 ;
      if (pwmValue > 100)
	pwmValue = 100 ;

      softPwmWrite (PWM_OUT_PIN, pwmValue) ;
      printf ("PWM -> %3d\n", pwmValue) ;

      while (digitalRead (PWM_UP) == LOW)
	delay (5) ;
    }

    if (digitalRead (PWM_DOWN) == LOW)
    {
      pwmValue -= 10 ;
      if (pwmValue < 0)
	pwmValue = 0 ;

      softPwmWrite (PWM_OUT_PIN, pwmValue) ;
      printf ("PWM -> %3d\n", pwmValue) ;

      while (digitalRead (PWM_DOWN) == LOW)
	delay (5) ;
    }

    delay (5) ;
  }

  return 0 ;
}
