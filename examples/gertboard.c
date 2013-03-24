/*
 * gertboard.c:
 *	Simple test for the SPI bus on the Gertboard
 *
 *	Hardware setup:
 *		D/A port 0 jumpered to A/D port 0.
 *
 *	We output a sine wave on D/A port 0 and sample A/D port 0. We then
 *	copy this value to D/A port 1 and use a 'scope on both D/A ports
 *	to check all's well.
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
#include <stdint.h>
#include <math.h>

#define	B_SIZE	200
#undef	DO_TIMING

#include <wiringPi.h>
#include <gertboard.h>

int main (void)
{
  double angle ;
  int i ;
  uint32_t x1 ;
  int  buffer [B_SIZE] ;

#ifdef	DO_TIMING
  unsigned int now, then ;
#endif

  printf ("Raspberry Pi Gertboard SPI test program\n") ;

  if (wiringPiSetupSys () < 0)
    return -1 ;

  if (gertboardSPISetup () < 0)
    return 1 ;

// Generate a Sine Wave

  for (i = 0 ; i < B_SIZE ; ++i)
  {
    angle = ((double)i / (double)B_SIZE) * M_PI * 2.0 ;
    buffer [i] = (int)rint ((sin (angle)) * 127.0 + 128.0) ;
  }


  for (;;)
  {
#ifdef	DO_TIMING
    then = millis () ;
#endif

    for (i = 0 ; i < B_SIZE ; ++i)
    {
      gertboardAnalogWrite (0, buffer [i]) ;

#ifndef	DO_TIMING
      x1 = gertboardAnalogRead (0) ;
      gertboardAnalogWrite (1, x1 >> 2) ;	// 10-bit A/D, 8-bit D/A
#endif
    }

#ifdef	DO_TIMING
    now = millis () ;
    printf ("%4d mS, %9.7f S/sample", now - then, ((double)(now - then) / 1000.0) / (double)B_SIZE) ;
    printf (" -> %9.4f samples/sec \n", 1 / (((double)(now - then) / 1000.0) / (double)B_SIZE)) ;
#endif
  }

  return 0 ;
}
