/*
 * piGlow1.c:
 *	Very simple demonstration of the PiGlow board.
 *	This uses the piGlow devLib.
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

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

#include <wiringPi.h>
#include <piGlow.h>

#define	PIGLOW_BASE	533

#ifndef	TRUE
#  define TRUE  (1==1)
#  define FALSE (!TRUE)
#endif


/*
 * keypressed: clearKeypressed:
 *	Simple but effective ways to tell if the enter key has been pressed
 *********************************************************************************
 */

static int keypressed (void)
{
  struct pollfd polls ;

  polls.fd     = fileno (stdin) ;
  polls.events = POLLIN ;

  return poll (&polls, 1, 0) != 0 ;
}

static void clearKeypressed (void)
{
  while (keypressed ())
    (void)getchar () ;
}


/*
 * pulseLed:
 *	Pulses the LED at position leg, ring from off to a max. value,
 *	then off again
 *********************************************************************************
 */

static void pulseLed (int leg, int ring)
{
  int i ;

  for (i = 0 ; i < 140 ; ++i)
  {
    piGlow1 (leg, ring, i) ;
    delay (1) ;
  }
  delay (10) ;
  for (i = 140 ; i >= 0 ; --i)
  {
    piGlow1 (leg, ring, i) ;
    delay (1) ;
  }
}

/*
 * pulseLeg:
 *	Same as above, but a whole leg at a time
 *********************************************************************************
 */

static void pulseLeg (int leg)
{
  int i ;

  for (i = 0 ; i < 140 ; ++i)
  {
    piGlowLeg (leg, i) ; delay (1) ;
  }
  delay (10) ;
  for (i = 140 ; i >= 0 ; --i)
  {
    piGlowLeg (leg, i) ; delay (1) ;
  }
}


/*
 * pulse Ring:
 *	Same as above, but a whole ring at a time
 *********************************************************************************
 */

static void pulseRing (int ring)
{
  int i ;

  for (i = 0 ; i < 140 ; ++i)
  {
    piGlowRing (ring, i) ; delay (1) ;
  }
  delay (10) ;
  for (i = 140 ; i >= 0 ; --i)
  {
    piGlowRing (ring, i) ; delay (1) ;
  }
}

#define	LEG_STEPS	3

static int legSequence [] =
{
   4, 12, 99,
  99,  4, 12, 
  12, 99,  4,
} ;
  

#define	RING_STEPS	16

static int ringSequence [] =
{
   0,  0,  0,  0,  0, 64,
   0,  0,  0,  0, 64, 64,
   0,  0,  0, 64, 64,  0,
   0,  0, 64, 64,  0,  0,
   0, 64, 64,  0,  0,  0,
  64, 64,  0,  0,  0,  0,
  64,  0,  0,  0,  0,  0,
   0,  0,  0,  0,  0,  0,
  64,  0,  0,  0,  0,  0,
  64, 64,  0,  0,  0,  0,
   0, 64, 64,  0,  0,  0,
   0,  0, 64, 64,  0,  0,
   0,  0,  0, 64, 64,  0,
   0,  0,  0,  0, 64, 64,
   0,  0,  0,  0,  0, 64,
   0,  0,  0,  0,  0,  0,
} ;

/*
 * main:
 *	Our little demo prgoram
 *********************************************************************************
 */

int main (void)
{
  int i ;
  int step, ring, leg ;

// Always initialise wiringPi:
//	Use the Sys method if you don't need to run as root

  wiringPiSetupSys () ;

// Initialise the piGlow devLib with our chosen pin base

  piGlowSetup (1) ;

// LEDs, one at a time

  printf ("LEDs, one at a time\n") ;
  for (; !keypressed () ;)
    for (leg = 0 ; leg < 3 ; ++leg)
    {
      for (ring = 0 ; ring < 6 ; ++ring)
      {
	pulseLed (leg, ring) ;
	if (keypressed ())
	  break ;
      }
      if (keypressed ())
	break ;
    }
  clearKeypressed () ;

// Rings, one at a time

  printf ("Rings, one at a time\n") ;
  for (; !keypressed () ;)
    for (ring = 0 ; ring < 6 ; ++ring)
    {
      pulseRing (ring) ;
      if (keypressed ())
	break ;
    }
  clearKeypressed () ;

// Legs, one at a time

  printf ("Legs, one at a time\n") ;
  for (; !keypressed () ;)
    for (leg = 0 ; leg < 3 ; ++leg)
    {
      pulseLeg (leg) ;
      if (keypressed ())
	break ;
    }
  clearKeypressed () ;

  delay (1000) ;

// Sequence - alternating rings, legs and random

  printf ("Sequence now\n") ;
  for (; !keypressed () ;)
  {
    for (i = 0 ; i < 20 ; ++i)
      for (step = 0 ; step < LEG_STEPS ; ++step)
      {
	for (leg = 0 ; leg < 3 ; ++leg)
	  piGlowLeg (leg, legSequence [step * 3 + leg]) ;
	delay (80) ;
      }

    for (i = 0 ; i < 10 ; ++i)
      for (step = 0 ; step < RING_STEPS ; ++step)
      {
	for (ring = 0 ; ring < 6 ; ++ring)
	  piGlowRing (ring, ringSequence [step * 6 + ring]) ;
	delay (80) ;
      }

    for (i = 0 ; i < 1000 ; ++i)
    {
      leg  = random () % 3 ;
      ring = random () % 6 ;
      piGlow1 (leg, ring, random () % 256) ;
      delay (5) ; 
      piGlow1 (leg, ring, 0) ;
    }
  }

  return 0 ;
}
