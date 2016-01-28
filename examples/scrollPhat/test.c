/*
 * test.c:
 *	Little test program forthe Pimoroni Scroll Phat.
 *
 * Copyright (c) 2015-2016 Gordon Henderson. <projects@drogon.net>
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <scrollPhat.h>


/*
 * prompt:
 *	Simple prompt & wait
 *********************************************************************************
 */

static void prompt (const char *p)
{
  printf ("    %s. Press ENTER: ", p) ;
  (void)getchar () ;
}


/*
 * the works
 *********************************************************************************
 */

int main (void)
{
  int x, y ;

  printf ("\n") ;
  printf ("Scroll Phat Test program\n") ;
  printf ("========================\n") ;

  if (scrollPhatSetup () != 0)
  {
    printf ("Unable to initialise the scrollPhat: %s\n", strerror (errno)) ;
    exit (1) ;
  }

  printf ("-> Scroll Phat initialised OK\n") ;
  printf ("... Basic display tests.\n\n") ;

  prompt ("Display ought to be blank") ;

// Light all pixels using one point at a time

  for (y = 0 ; y < 5 ; ++y)
    for (x = 0 ; x < 12 ; ++x)
      scrollPhatPoint (x, y, 1) ;
  scrollPhatUpdate () ;

  prompt ("Display ought to be all lit-up") ;

// Big rectangle
  
  scrollPhatClear () ;
  scrollPhatRectangle (0,0, 10, 4, 1, 0) ;
  scrollPhatUpdate () ;

  prompt ("There should now be a rectangle round the outside") ;

  scrollPhatLine (0,0, 10,4, 1) ;
  scrollPhatLine (0,4, 10,0, 1) ;
  scrollPhatUpdate () ;

  prompt ("Diagonal lines") ;

  scrollPhatIntensity (1) ;

  prompt ("Minimum brightness") ;
  
  scrollPhatIntensity (100) ;

  prompt ("Maximum brightness") ;
  
  scrollPhatIntensity (10) ;

  prompt ("Default brightness") ;
  
  scrollPhatClear () ;
  
  printf ("    Message Test...Press Ctrl-C to exit: ") ;
  fflush (stdout) ;

  scrollPhatPrintSpeed (75) ;
  for (;;)
    scrollPhatPuts ("  Welcome to the scroll phat from Pimoroni  ") ;
  
  return 0 ;
}
