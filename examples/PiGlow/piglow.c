/*
 * piglow.c:
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
#include <string.h>
#include <stdlib.h>

#ifndef	TRUE
#  define TRUE  (1==1)
#  define FALSE (!TRUE)
#endif

#include <wiringPi.h>
#include <piGlow.h>

static void failUsage (void)
{
  fprintf (stderr, "Usage examples:\n") ;
  fprintf (stderr, "  piglow off         # All off\n") ;
  fprintf (stderr, "  piglow red 50      # Light the 3 red LEDs to 50%%\n") ;
  fprintf (stderr, "     colours are: red, yellow, orange, green, blue and white\n") ;
  fprintf (stderr, "  piglow all 75      # Light all to 75%%\n") ;
  fprintf (stderr, "  piglow leg 0 25    # Light leg 0 to 25%%\n") ;
  fprintf (stderr, "  piglow ring 3 100  # Light ring 3 to 100%%\n") ;
  fprintf (stderr, "  piglow led 2 5 100 # Light the single LED on Leg 2, ring 5 to 100%%\n") ;

  exit (EXIT_FAILURE) ;
}

static int getPercent (char *typed)
{
  int percent ;

  percent = atoi (typed) ;
  if ((percent < 0) || (percent > 100))
  {
    fprintf (stderr, "piglow: percent value out of range\n") ;
    exit (EXIT_FAILURE) ;
  }
  return (percent * 255) / 100 ;
}


/*
 * main:
 *	Our little demo prgoram
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int percent ;
  int ring, leg ;

// Always initialise wiringPi:
//	Use the Sys method if you don't need to run as root

  wiringPiSetupSys () ;

// Initialise the piGlow devLib

  piGlowSetup (FALSE) ;

  if (argc == 1)
    failUsage () ;

  if ((argc == 2) && (strcasecmp (argv [1], "off") == 0))
  {
    for (leg = 0 ; leg < 3 ; ++leg)
      piGlowLeg (leg, 0) ;
    return 0 ;
  }

  if (argc == 3)
  {
    percent = getPercent (argv [2]) ;

    /**/ if (strcasecmp (argv [1], "red") == 0)
      piGlowRing (PIGLOW_RED, percent) ;
    else if (strcasecmp (argv [1], "yellow") == 0)
      piGlowRing (PIGLOW_YELLOW, percent) ;
    else if (strcasecmp (argv [1], "orange") == 0)
      piGlowRing (PIGLOW_ORANGE, percent) ;
    else if (strcasecmp (argv [1], "green") == 0)
      piGlowRing (PIGLOW_GREEN, percent) ;
    else if (strcasecmp (argv [1], "blue") == 0)
      piGlowRing (PIGLOW_BLUE, percent) ;
    else if (strcasecmp (argv [1], "white") == 0)
      piGlowRing (PIGLOW_WHITE, percent) ;
    else if (strcasecmp (argv [1], "all") == 0)
      for (ring = 0 ; ring < 6 ; ++ring)
	piGlowRing (ring, percent) ;
    else
    {
      fprintf (stderr, "piglow: invalid colour\n") ;
      exit (EXIT_FAILURE) ;
    }
    return 0 ;
  }

  if (argc == 4)
  {
    /**/ if (strcasecmp (argv [1], "leg") == 0)
    {
      leg = atoi (argv [2]) ;
      if ((leg < 0) || (leg > 2))
      {
	fprintf (stderr, "piglow: leg value out of range\n") ;
	exit (EXIT_FAILURE) ;
      }
      percent = getPercent (argv [3]) ;
      piGlowLeg (leg, percent) ;
    }
    else if (strcasecmp (argv [1], "ring") == 0)
    {
      ring = atoi (argv [2]) ;
      if ((ring < 0) || (ring > 5))
      {
	fprintf (stderr, "piglow: ring value out of range\n") ;
	exit (EXIT_FAILURE) ;
      }
      percent = getPercent (argv [3]) ;
      piGlowRing (ring, percent) ;
    }
    return 0 ;
  }

  if (argc == 5)
  {
    if (strcasecmp (argv [1], "led") != 0)
      failUsage () ;

    leg = atoi (argv [2]) ;
    if ((leg < 0) || (leg > 2))
    {
      fprintf (stderr, "piglow: leg value out of range\n") ;
      exit (EXIT_FAILURE) ;
    }
    ring = atoi (argv [3]) ;
    if ((ring < 0) || (ring > 5))
    {
      fprintf (stderr, "piglow: ring value out of range\n") ;
      exit (EXIT_FAILURE) ;
    }
    percent = getPercent (argv [4]) ;
    piGlow1 (leg, ring, percent) ;
    return 0 ;
  }

  failUsage () ;
  return 0 ; 
}


