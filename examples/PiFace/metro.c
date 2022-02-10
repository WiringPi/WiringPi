/*
 * metronome.c:
 *	Simple test for the PiFace interface board.
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
#include <stdlib.h>
#include <string.h>

#include <wiringPi.h>
#include <piFace.h>

#define	PIFACE	200

/*
 * middleA:
 *	Play middle A (on the relays - yea!)
 *********************************************************************************
 */

static void middleA (void)
{
  unsigned int next ;

  for (;;)
  {
    next = micros () + 1136 ;
    digitalWrite (PIFACE + 0, 0)  ;
    digitalWrite (PIFACE + 1, 0)  ;
    while (micros () < next)
      delayMicroseconds (1) ;
    
    next = micros () + 1137 ;
    digitalWrite (PIFACE + 0, 1)  ;
    digitalWrite (PIFACE + 1, 1)  ;
    while (micros () < next)
      delayMicroseconds (1) ;
    
  }
}


int main (int argc, char *argv [])
{
  int bpm, msPerBeat, state = 0 ;
  unsigned int end ;

  printf ("Raspberry Pi PiFace Metronome\n") ;
  printf ("=============================\n") ;

  piHiPri (50) ;

  wiringPiSetupSys () ;	// Needed for timing functions
  piFaceSetup      (PIFACE) ;

  if (argc != 2)
  {
    printf ("Usage: %s <beates per minute>\n", argv [0]) ;
    exit (1) ;
  }

  if (strcmp (argv [1], "a") == 0)
    middleA () ;

  bpm = atoi (argv [1]) ;

  if ((bpm < 40) || (bpm > 208))
  {
    printf ("%s range is 40 through 208 beats per minute\n", argv [0]) ;
    exit (1) ;
  }

  msPerBeat = 60000 / bpm  ;

// Main loop:
//	Put some random LED pairs up for a few seconds, then blank ...

  for (;;)
  {
    end = millis ()  + msPerBeat ;

    digitalWrite (PIFACE + 0, state)  ;
    digitalWrite (PIFACE + 1, state)  ;

    while (millis () < end)
      delayMicroseconds (500) ;

    state ^= 1 ;
  }

  return 0 ;
}
