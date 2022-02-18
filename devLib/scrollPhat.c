/*
 * scrollPhat.c:
 *	Simple driver for the Pimoroni Scroll Phat device
 *
 * Copyright (c) 2015 Gordon Henderson.
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
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include <wiringPiI2C.h>

#include "scrollPhatFont.h"
#include "scrollPhat.h"

// Size

#define	SP_WIDTH	11
#define	SP_HEIGHT	 5

// I2C

#define	PHAT_I2C_ADDR	0x60

// Software copy of the framebuffer
//	it's 8-bit deep although the display itself is only 1-bit deep.

static unsigned char frameBuffer [SP_WIDTH * SP_HEIGHT] ;

static int lastX,   lastY ;
static int printDelayFactor  ;
static int scrollPhatFd ;

static int putcharX ;

#undef	DEBUG


/*
 * delay:
 *	Wait for some number of milliseconds.
 *	This taken from wiringPi as there is no-need to include the whole of
 *	wiringPi just for the delay function.
 *********************************************************************************
 */

static void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}



/*
 * scrollPhatUpdate:
 *	Copy our software version to the real display
 *********************************************************************************
 */

void scrollPhatUpdate (void)
{
  register int x, y ;
  register unsigned char data, pixel ;
  unsigned char pixels [SP_WIDTH] ;

#ifdef	DEBUG
  printf ("+-----------+\n") ;
  for (y = 0 ; y < SP_HEIGHT ; ++y)
  {
    putchar ('|') ;
    for (x = 0 ; x < SP_WIDTH ; ++x)
    {
      pixel = frameBuffer [x + y * SP_WIDTH] ;
      putchar (pixel == 0 ? ' ' : '*') ;
    }
    printf ("|\n") ;
  }
  printf ("+-----------+\n") ;
#endif 

  for (x = 0 ; x < SP_WIDTH ; ++x)
  {
    data = 0 ;
    for (y = 0 ; y < SP_HEIGHT ; ++y)
    {
      pixel = frameBuffer [x + y * SP_WIDTH] ;
      data = (data << 1) | ((pixel == 0) ? 0 : 1) ;
    }
    pixels [x] = data ;
  }

  for (x = 0 ; x < SP_WIDTH ; ++x)
    wiringPiI2CWriteReg8 (scrollPhatFd, 1 + x, pixels [x]) ;

  wiringPiI2CWriteReg8 (scrollPhatFd, 0x0C, 0) ;
}


/*
 *********************************************************************************
 * Standard Graphical Functions
 *********************************************************************************
 */


/*
 * scrollPhatPoint:
 *	Plot a pixel. Crude clipping - speed is not the essence here.
 *********************************************************************************
 */

void scrollPhatPoint (int x, int y, int colour)
{
  lastX = x ;
  lastY = y ;

  if ((x < 0) || (x >= SP_WIDTH) || (y < 0) || (y >= SP_HEIGHT))
    return ;

  frameBuffer [x + y * SP_WIDTH] = colour ;
}


/*
 * scrollPhatLine: scrollPhatLineTo:
 *	Classic Bressenham Line code - rely on the point function to do the
 *	clipping for us here.
 *********************************************************************************
 */

void scrollPhatLine (int x0, int y0, int x1, int y1, int colour)
{
  int dx, dy ;
  int sx, sy ;
  int err, e2 ;

  lastX = x1 ;
  lastY = y1 ;

  dx = abs (x1 - x0) ;
  dy = abs (y1 - y0) ;

  sx = (x0 < x1) ? 1 : -1 ;
  sy = (y0 < y1) ? 1 : -1 ;

  err = dx - dy ;
 
  for (;;)
  {
    scrollPhatPoint (x0, y0, colour) ;

    if ((x0 == x1) && (y0 == y1))
      break ;

    e2 = 2 * err ;

    if (e2 > -dy)
    {
      err -= dy ;
      x0  += sx ;
    }

    if (e2 < dx)
    {
      err += dx ;
      y0  += sy ;
    }
  }

}

void scrollPhatLineTo (int x, int y, int colour)
{
  scrollPhatLine (lastX, lastY, x, y, colour) ;
}


/*
 * scrollPhatRectangle:
 *	A rectangle is a spoilt days fishing
 *********************************************************************************
 */

void scrollPhatRectangle (int x1, int y1, int x2, int y2, int colour, int filled)
{
  register int x ;

  if (filled)
  {
    /**/ if (x1 == x2)
      scrollPhatLine (x1, y1, x2, y2, colour) ;
    else if (x1 < x2)
      for (x = x1 ; x <= x2 ; ++x)
	scrollPhatLine (x, y1, x, y2, colour) ;
    else
      for (x = x2 ; x <= x1 ; ++x)
	scrollPhatLine (x, y1, x, y2, colour) ;
  }
  else
  {
    scrollPhatLine   (x1, y1, x2, y1, colour) ;
    scrollPhatLineTo (x2, y2, colour) ;
    scrollPhatLineTo (x1, y2, colour) ;
    scrollPhatLineTo (x1, y1, colour) ;
  }
}


/*
 * scrollPhatPutchar:
 *      Print a single character to the screen then advance the pointer by an
 *	appropriate ammount (variable width font).
 *      We rely on the clipping done by the pixel plot function to keep us
 *      out of trouble.
 *	Return the width + space
 *********************************************************************************
 */

int scrollPhatPutchar (int c)
{
  register int x, y ;

  unsigned char line ;
  unsigned char *fontPtr ;
  unsigned char *p2 ;
  int lineWidth, width, mask ;

// The font is printable characters, uppercase only...
//	and somewhat varaible width...

  c &= 0x7F ;
  if (c > 0x60)
    c -= 64 ;
  else
    c -= 32 ;

  fontPtr = scrollPhatFont + c * fontHeight ;

// Work out width of this character
//	There probably is a more efficient way to do this, but...

  p2    = fontPtr ;
  width = 0 ;
  for (y = 0 ; y < fontHeight ; ++y)
  {
    mask = 0x80 ;
    for (lineWidth = 8 ; lineWidth > 0 ; --lineWidth)
    {
      if ((*p2 & mask) != 0)
	break ;
      mask >>= 1 ;
    }
    if (lineWidth > width)
      width = lineWidth ;

    ++p2 ;
  }

  if (width == 0)	// Likely to be a blank or space character
    width = 3 ;

  for (y = fontHeight - 1 ; y >= 0 ; --y)
  {
    x    = 0 ;
    line = *fontPtr++ ;
    for (mask = 1 << (width - 1) ; mask != 0 ; mask >>= 1)
    {
      scrollPhatPoint (putcharX + x, y, (line & mask)) ;
      ++x ;
    }
  }

// make a line of space

  for (y = fontHeight - 1 ; y >= 0 ; --y)
    scrollPhatPoint (putcharX + width, y, 0) ;

  putcharX = putcharX + width + 1 ;

  return width + 1 ;
}


/*
 * scrollPhatPuts:
 *	Send a string to the display - and scroll it across.
 *	This is somewhat of a hack in that we print the entire string to the
 *	display and let the point clipping take care of what's off-screen...
 *********************************************************************************
 */

void scrollPhatPuts (const char *str)
{
  int i ;
  int movingX = 0 ;
  const char *s ;
  int pixelLen ;

// Print it once, then we know the width in pixels...

  putcharX = 0 ;
  s = str ;
  while (*s)
    scrollPhatPutchar (*s++) ;

  pixelLen = putcharX ;

// Now scroll it by printing it and moving left one pixel

  movingX = 0 ;
  for (i = 0 ; i < pixelLen ; ++i)
  {
    putcharX = movingX ;
    s = str ;
    while (*s)
      scrollPhatPutchar (*s++) ;
    --movingX ;
    scrollPhatUpdate () ;
    delay (printDelayFactor) ;
  }
}


/*
 * scrollPhatPrintf:
 *	Does what it says
 *********************************************************************************
 */

void scrollPhatPrintf (const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  scrollPhatPuts (buffer) ;
}


/*
 * scrollPhatPrintSpeed:
 *	Change the print speed - mS per shift by 1 pixel
 *********************************************************************************
 */

void scrollPhatPrintSpeed (const int pps)
{
  if (pps < 0)
    printDelayFactor = 0 ;
  else
    printDelayFactor = pps ;
}


/*
 * scrollPhatClear:
 *	Clear the display
 *********************************************************************************
 */

void scrollPhatClear (void)
{
  register int i ;
  register unsigned char *ptr = frameBuffer ;

  for (i = 0 ; i < (SP_WIDTH * SP_HEIGHT) ; ++i)
    *ptr++ = 0 ;

  scrollPhatUpdate () ;
}


/*
 * scrollPhatIntensity:
 *	Set the display brightness - percentage
 *********************************************************************************
 */

void scrollPhatIntensity (const int percent)
{
  wiringPiI2CWriteReg8 (scrollPhatFd, 0x19, (127 * percent) / 100) ;
}


/*
 * scrollPhatSetup:
 *	Initialise the Scroll Phat display
 *********************************************************************************
 */

int scrollPhatSetup (void)
{
  if ((scrollPhatFd = wiringPiI2CSetup (PHAT_I2C_ADDR)) < 0)
    return scrollPhatFd ;

  wiringPiI2CWriteReg8 (scrollPhatFd, 0x00, 0x03) ;	// Enable display, set to 5x11 mode
  scrollPhatIntensity (10) ;
  scrollPhatClear () ;
  scrollPhatPrintSpeed (100) ;

  return 0 ;
}
