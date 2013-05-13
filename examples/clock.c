/*
 * clock.c:
 *	Demo of the 128x64 graphics based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based on the popular 12864H controller chip.
 *
 *	This test program assumes the following:
 *		(Which is currently hard-wired into the driver)
 *
 *	GPIO 0-7 is connected to display data pins 0-7.
 *	GPIO 10 is CS1
 *	GPIO 11 is CS2
 *	GPIO 12 is STROBE
 *	GPIO 10 is RS
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
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
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include <wiringPi.h>
#include <lcd128x64.h>

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

double clockRadius ;
double thickness, barLen ;
int maxX, maxY ;

double rads (double degs)
{
  return degs * M_PI / 180.0 ;
}

void drawClockHands (void)
{
  time_t t ;
  struct tm *now ;
  double angle, p, x0, y0, x1, y1 ;
  int h24, h, m, s ;
  char text [20] ;

  time (&t) ;
  now = localtime (&t) ;

  h24 = now->tm_hour ;
  m   = now->tm_min ;
  s   = now->tm_sec ;

  h = h24 ;
  if (h > 12)
    h -= 12 ;

// Hour hand

  angle = h * 30 + m * 0.5 ;
  x0 = sin (rads (angle)) * (clockRadius * 0.75) ;
  y0 = cos (rads (angle)) * (clockRadius * 0.75) ;
  for (p = -3.0 ; p <= 3.0 ; p += 0.2)
  {
    x1 = sin (rads (angle + p)) * (clockRadius * 0.7) ;
    y1 = cos (rads (angle + p)) * (clockRadius * 0.7) ;
    lcd128x64line (0, 0, x1, y1, 1) ;
    lcd128x64lineTo (x0, y0, 1) ;
  }

// Minute hand

  angle = m * 6 ;
  x0 = sin (rads (angle)) * (clockRadius * 0.9) ;
  y0 = cos (rads (angle)) * (clockRadius * 0.9) ;
  for (p = -1.0 ; p <= 1.0 ; p += 0.2)
  {
    x1 = sin (rads (angle + p)) * (clockRadius * 0.85) ;
    y1 = cos (rads (angle + p)) * (clockRadius * 0.85) ;
    lcd128x64line (0, 0, x1, y1, 1) ;
    lcd128x64lineTo (x0, y0, 1) ;
  }

// Second hand

  angle = s * 6 ;
  x0 = sin (rads (angle)) * (clockRadius * 0.2) ;
  y0 = cos (rads (angle)) * (clockRadius * 0.2) ;
  x1 = sin (rads (angle)) * (clockRadius * 0.95) ;
  y1 = cos (rads (angle)) * (clockRadius * 0.95) ;
  lcd128x64line (0 - x0, 0 - y0, x1, y1, 1) ;
  lcd128x64circle (0, 0, clockRadius * 0.1,  0, 1) ;
  lcd128x64circle (0, 0, clockRadius * 0.05, 1, 1) ;

// Text:

  sprintf (text, "%02d:%02d:%02d", h24, m, s) ;
  lcd128x64puts (32, 24, text, 0, 1) ;

  sprintf (text, "%2d/%2d/%2d", now->tm_mday, now->tm_mon + 1, now->tm_year - 100) ;
  lcd128x64puts (32, -23, text, 0, 1) ;
}

void drawClockFace (void)
{
  int m ;
  double d, px1, py1, px2, py2 ;

  lcd128x64clear (0) ;
  lcd128x64circle (0,0, clockRadius, 1, TRUE) ;
  lcd128x64circle (0,0, clockRadius - thickness, 0, TRUE) ;

// The four big indicators for 12,15,30 and 45

  lcd128x64rectangle (- 3,  clockRadius - barLen, 3,  clockRadius,     1, TRUE) ;	// 12
  lcd128x64rectangle (clockRadius - barLen, 3,  clockRadius, -3, 1, TRUE) ;	// 3
  lcd128x64rectangle (- 3, -clockRadius + barLen, 3, -clockRadius, 1, TRUE) ;	// 6
  lcd128x64rectangle (-clockRadius + barLen, 3, -clockRadius, -3, 1, TRUE) ;	// 9


// Smaller 5 and 1 minute ticks

  for (m = 0 ; m < 60 ; ++m)
  {
    px1 = sin (rads (m * 6)) * clockRadius ;
    py1 = cos (rads (m * 6)) * clockRadius ;
    if ((m % 5) == 0)
      d = barLen ;
    else 
      d = barLen / 2.0 ;

    px2 = sin (rads (m * 6)) * (clockRadius - d) ;
    py2 = cos (rads (m * 6)) * (clockRadius - d) ;
    lcd128x64line (px1, py1, px2, py2, 1) ;
  }
}

void setup (void)
{
  lcd128x64getScreenSize (&maxX, &maxY) ;
  clockRadius = maxY / 2 - 1 ;
  thickness = maxX / 48 ;
  barLen = thickness * 4 ;
  lcd128x64setOrigin (32, 32) ;
}




/*
 ***********************************************************************
 * The main program
 ***********************************************************************
 */

int main (int argc, char *argv [])
{
  time_t now ;

  wiringPiSetup () ;

  lcd128x64setup () ;

  setup () ;
  for (;;)
  {
    drawClockFace  () ;
    drawClockHands () ;
    lcd128x64update () ;

    now = time (NULL) ;
    while (time (NULL) == now)
      delay (10) ;
  }
  

  return 0 ;
}
