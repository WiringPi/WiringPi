/*
 * piNes.c:
 *	Driver for the NES Joystick controller on the Raspberry Pi
 *	Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <wiringPi.h>

#include "piNes.h"

#define	MAX_NES_JOYSTICKS	8

#define	NES_RIGHT	0x01
#define	NES_LEFT	0x02
#define	NES_DOWN	0x04
#define	NES_UP		0x08
#define	NES_START	0x10
#define	NES_SELECT	0x20
#define	NES_B		0x40
#define	NES_A		0x80


#define	PULSE_TIME	25

// Data to store the pins for each controller

struct nesPinsStruct
{
  unsigned int cPin, dPin, lPin ;
} ;

static struct nesPinsStruct nesPins [MAX_NES_JOYSTICKS] ;

static int joysticks = 0 ;


/*
 * setupNesJoystick:
 *	Create a new NES joystick interface, program the pins, etc.
 *********************************************************************************
 */

int setupNesJoystick (int dPin, int cPin, int lPin)
{
  if (joysticks == MAX_NES_JOYSTICKS)
    return -1 ;

  nesPins [joysticks].dPin = dPin ;
  nesPins [joysticks].cPin = cPin ;
  nesPins [joysticks].lPin = lPin ;

  digitalWrite (lPin, LOW) ;
  digitalWrite (cPin, LOW) ;

  pinMode (lPin, OUTPUT) ;
  pinMode (cPin, OUTPUT) ;
  pinMode (dPin, INPUT) ;

  return joysticks++ ;
}


/*
 * readNesJoystick:
 *	Do a single scan of the NES Joystick.
 *********************************************************************************
 */

unsigned int readNesJoystick (int joystick)
{
  unsigned int value = 0 ;
  int  i ;

  struct nesPinsStruct *pins = &nesPins [joystick] ;
 
// Toggle Latch - which presents the first bit

  digitalWrite (pins->lPin, HIGH) ; delayMicroseconds (PULSE_TIME) ;
  digitalWrite (pins->lPin, LOW)  ; delayMicroseconds (PULSE_TIME) ;

// Read first bit

  value = digitalRead (pins->dPin) ;

// Now get the next 7 bits with the clock

  for (i = 0 ; i < 7 ; ++i)
  {
    digitalWrite (pins->cPin, HIGH) ; delayMicroseconds (PULSE_TIME) ;
    digitalWrite (pins->cPin, LOW)  ; delayMicroseconds (PULSE_TIME) ;
    value = (value << 1) | digitalRead (pins->dPin) ;
  }

  return value ^ 0xFF ;
}
