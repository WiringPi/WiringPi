/*
 * piGlow.c:
 *	Easy access to the Pimoroni PiGlow board.
 *
 * Copyright (c) 2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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
#include <sn3218.h>

#include "piGlow.h"

#define	PIGLOW_BASE	577

static int leg0 [6] = {  6,  7,  8,  5,  4,  9 } ;
static int leg1 [6] = { 17, 16, 15, 13, 11, 10 } ;
static int leg2 [6] = {  0,  1,  2,  3, 14, 12 } ;


/*
 * piGlow1:
 *	Light up an individual LED
 *********************************************************************************
 */

void piGlow1 (const int leg, const int ring, const int intensity)
{
  int *legLeds ;

  if ((leg  < 0) || (leg  > 2)) return ;
  if ((ring < 0) || (ring > 5)) return ;

  /**/ if (leg == 0)
    legLeds = leg0 ;
  else if (leg == 1)
    legLeds = leg1 ;
  else
    legLeds = leg2 ;

  analogWrite (PIGLOW_BASE + legLeds [ring], intensity) ;
}

/*
 * piGlowLeg:
 *	Light up all 6 LEDs on a leg
 *********************************************************************************
 */

void piGlowLeg (const int leg, const int intensity)
{
  int  i ;
  int *legLeds ;

  if ((leg < 0) || (leg > 2))
    return ;

  /**/ if (leg == 0)
    legLeds = leg0 ;
  else if (leg == 1)
    legLeds = leg1 ;
  else
    legLeds = leg2 ;

  for (i = 0 ; i < 6 ; ++i)
    analogWrite (PIGLOW_BASE + legLeds [i], intensity) ;
}


/*
 * piGlowRing:
 *	Light up 3 LEDs in a ring. Ring 0 is the outermost, 5 the innermost
 *********************************************************************************
 */

void piGlowRing (const int ring, const int intensity)
{
  if ((ring < 0) || (ring > 5))
    return ;

  analogWrite (PIGLOW_BASE + leg0 [ring], intensity) ;
  analogWrite (PIGLOW_BASE + leg1 [ring], intensity) ;
  analogWrite (PIGLOW_BASE + leg2 [ring], intensity) ;
}

/*
 * piGlowSetup:
 *	Initialise the board & remember the pins we're using
 *********************************************************************************
 */

void piGlowSetup (int clear)
{
  sn3218Setup (PIGLOW_BASE) ;

  if (clear)
  {
    piGlowLeg (0, 0) ;
    piGlowLeg (1, 0) ;
    piGlowLeg (2, 0) ;
  }
}
