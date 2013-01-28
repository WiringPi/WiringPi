/*
 * delayTest.c:
 *	Just a little test program I'm using to experiment with
 *	various timings and latency, etc.
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

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#include <sys/time.h>

#define	CYCLES	1000

int main()
{
  int x ;
  struct timeval t1, t2 ;
  int t ;
  int max, min ;
  int del ;
  int underRuns, overRuns, exactRuns, total ;
  int descheds ;

  if (wiringPiSetup () == -1)
    return 1 ;

  piHiPri (10) ; sleep (1) ;

// Baseline test

  gettimeofday (&t1, NULL) ;
  gettimeofday (&t2, NULL) ;

  t = t2.tv_usec - t1.tv_usec ;
  printf ("Baseline test: %d\n", t);

  for (del = 1 ; del < 200 ; ++del)
  {
    underRuns = overRuns = exactRuns = total = 0 ;
    descheds = 0 ;
    max = del ;
    min = del ;

    for (x = 0 ; x < CYCLES ; ++x)
    {
      for (;;)				// Repeat this if we get a delay over 999uS
      {					// -> High probability Linux has deschedulled us
	gettimeofday (&t1, NULL) ;
	  delayMicroseconds (del) ;
	gettimeofday (&t2, NULL) ;

	if (t2.tv_usec < t1.tv_usec)	// Counter wrapped
	  t = (1000000 + t2.tv_usec) - t1.tv_usec;
	else
	  t = t2.tv_usec - t1.tv_usec ;
	if (t > 999)
	{
	  ++descheds ;
	  continue ;
	}
	else
	  break ;
      }

      if (t > max)
      {
        max = t ;
	++overRuns ;
      }
      else if (t < min)
      {
	min = t ;
	++underRuns ;
      }
      else
	++exactRuns ;

      total += t ;
    }
    printf ("Delay: %3d. Min: %3d, Max: %3d, Unders: %3d, Overs: %3d, Exacts: %3d, Average: %3d,  Descheds: %2d\n",
	del, min, max, underRuns, overRuns, exactRuns, total / CYCLES,  descheds) ;
    fflush (stdout) ;
    delay (1) ;
  }

  return 0 ;
}
