/*
 * rht03.c:
 *	Driver for the MaxDetect series sensors
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

#include <wiringPi.h>
#include <maxdetect.h>

#define	RHT03_PIN	7

/*
 ***********************************************************************
 * The main program
 ***********************************************************************
 */

int main (void)
{
  int result, temp, rh ;
  int minT, maxT, minRH, maxRH ;

  int numGood, numBad ;

  wiringPiSetup () ;
  piHiPri       (55) ;

  minT =  1000 ;
  maxT = -1000 ;

  minRH =  1000 ;
  maxRH = -1000 ;

  numGood = numBad = 0 ;

  for (;;)
  {
    delay (100) ;

    result = readRHT03 (RHT03_PIN, &temp, &rh) ;

    if (!result)
    {
      printf (".") ;
      fflush (stdout) ;
      ++numBad ;
      continue ;
    }

    ++numGood ;

    if (temp < minT) minT = temp ;
    if (temp > maxT) maxT = temp ;
    if (rh  < minRH) minRH = rh ;
    if (rh  > maxRH) maxRH = rh ;

    printf ("\r%6d, %6d: ", numGood, numBad) ;
    printf ("Temp: %5.1f, RH: %5.1f%%", temp / 10.0, rh / 10.0) ;
    printf ("  Max/Min Temp: %5.1f:%5.1f", maxT / 10.0, minT / 10.0) ;
    printf ("  Max/Min RH: %5.1f:%5.1f", maxRH / 10.0, minRH / 10.0) ;

    printf ("\n") ;
  }

  return 0 ;
}
