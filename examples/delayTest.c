/*
 * delayTest.c:
 *	Just a little test program I'm using to experiment with
 *	various timings and latency, etc.
 *
 * Copyright (c) 2012-2013 Gordon Henderson, 2025 Contributors
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

 // compile: gcc -Wall delayTest.c -o delayTest -lwiringPi

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <sys/time.h>

#define	CYCLES	1000

int main()
{
  struct timeval tStart, tStop, tDuration ;
  int t ;
  int max, min ;
  int del ;
  int underRuns, overRuns, exactRuns, total ;
  int descheds ;


// Baseline test

  gettimeofday (&tStart, NULL) ;
  gettimeofday (&tStop, NULL) ;

  t = tStop.tv_usec - tStart.tv_usec ;
  printf ("Baseline test (no sleep took): %d usec\n", t);
  printf ("Start delayTest (delayMicroseconds) 1-200 usec with %d iteration:\n", CYCLES);
  for (del = 1 ; del <= 200 ; ++del)
  {
    underRuns = overRuns = exactRuns = total = 0 ;
    descheds = 0 ;
    max =   0 ;
    min = 999 ;

    for (int iter = 0 ; iter < CYCLES ; ++iter)
    {
      for (;;)				// Repeat this if we get a delay over 999uS
      {					// -> High probability Linux has deschedulled us
        gettimeofday (&tStart, NULL) ;
	      //usleep (del) ;
        delayMicroseconds (del) ;
	      gettimeofday (&tStop, NULL) ;
	      timersub (&tStop, &tStart, &tDuration) ;
	      t = tDuration.tv_usec ;

	      if (t > 999)
	      {
	        ++descheds ;
	        continue ;
	      }
	      else
	        break ;
      }

      if (t == del)
	      ++exactRuns ;
      else if (t < del)
	      ++underRuns ;
      else if (t > del)
	      ++overRuns ;

      if (t > max)
        max = t ;
      else if (t < min)
	      min = t ;

      total += t ;
    }
    int aver = total / CYCLES;
    printf ("Delay: %3d usec. Min: %3d, Max: %3d, Unders: %3d%%, Overs: %3d%%, Exacts: %3d%%, Average: %3d (abs dev: %2d) ,  Descheds: %2d\n",
	   del, min-del, max-del, underRuns*100/CYCLES, overRuns*100/CYCLES, exactRuns*100/CYCLES, aver, aver-del,  descheds) ;
    fflush (stdout) ;
    usleep (1000) ;
  }

  return 0 ;
}
