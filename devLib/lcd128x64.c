/*
 * lcd128x64.c:
 *	Graphics-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based on the generic 12864H chips
 *
 *	There are many variations on these chips, however they all mostly
 *	seem to be similar.
 *	This implementation has the Pins from the Pi hard-wired into it,
 *	in particular wiringPi pins 0-7 so that we can use
 *	digitalWriteByete() to speed things up somewhat.
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

#include <wiringPi.h>

#include "font.h"
#include "lcd128x64.h"

// Size

#define	LCD_WIDTH	128
#define	LCD_HEIGHT	 64

// Hardware Pins
//	Note pins 0-7 are the 8-bit data port

#define	CS1		10
#define	CS2		11
#define	STROBE		12
#define	RS		13

// Software copy of the framebuffer
//	it's 8-bit deep although the display itself is only 1-bit deep.

static unsigned char frameBuffer [LCD_WIDTH * LCD_HEIGHT] ;

static int maxX,    maxY ;
static int lastX,   lastY ;
static int xOrigin, yOrigin ;
static int lcdOrientation = 0 ;

/*
 * strobe:
 *	Toggle the strobe (Really the "E") pin to the device.
 *	According to the docs, data is latched on the falling edge.
 *********************************************************************************
 */

static void strobe (void)
{
  digitalWrite (STROBE, 1) ; delayMicroseconds (1) ;
  digitalWrite (STROBE, 0) ; delayMicroseconds (5) ;
}


/*
 * sentData:
 *	Send an data or command byte to the display.
 *********************************************************************************
 */

static void sendData (const int data, const int chip)
{
  digitalWrite     (chip, 0) ;
  digitalWriteByte (data) ;
  strobe           () ;
  digitalWrite     (chip, 1) ;
}


/*
 * sendCommand:
 *	Send a command byte to the display
 *********************************************************************************
 */

static void sendCommand (const int command, const int chip)
{
  digitalWrite (RS, 0) ;
  sendData     (command, chip) ;
  digitalWrite (RS, 1) ;
}


/*
 * setCol: SetLine:
 *	Set the column and line addresses
 *********************************************************************************
 */

static void setCol  (int col, const int chip)
  { sendCommand (0x40 | (col  & 0x3F), chip) ; }

static void setLine (int line, const int chip)
  { sendCommand (0xB8 | (line & 0x07), chip) ; }


/*
 * lcd128x64update:
 *	Copy our software version to the real display
 *********************************************************************************
 */

void lcd128x64update (void)
{
  int line, x, y, fbLoc ;
  unsigned char byte ;

// Left side 

  for (line = 0 ; line < 8 ; ++line)
  {
    setCol  (0,    CS1) ;
    setLine (line, CS1) ;

    for (x = 63 ; x >= 0 ; --x)
    {
      byte = 0 ;
      for (y = 0 ; y < 8 ; ++y)
      {
	fbLoc = x + (((7 - line) * 8) + (7 - y)) * LCD_WIDTH ;
	if (frameBuffer [fbLoc] != 0)
	  byte |= (1 << y) ;
      }
      sendData (byte, CS1) ;
    }
  }

// Right side 

  for (line = 0 ; line < 8 ; ++line)
  {
    setCol  (0,    CS2) ;
    setLine (line, CS2) ;

    for (x = 127 ; x >= 64 ; --x)
    {
      byte = 0 ;
      for (y = 0 ; y < 8 ; ++y)
      {
	fbLoc = x + (((7 - line) * 8) + (7 - y)) * LCD_WIDTH ;
	if (frameBuffer [fbLoc] != 0)
	  byte |= (1 << y) ;
      }
      sendData (byte, CS2) ;
    }
  }
}


/*
 * lcd128x64setOrigin:
 *	Set the display offset origin
 *********************************************************************************
 */

void lcd128x64setOrigin (int x, int y)
{
  xOrigin = x ;
  yOrigin = y ;
}


/*
 * lcd128x64setOrientation:
 *	Set the display orientation:
 *	0: Normal, the display is portrait mode, 0,0 is top left
 *	1: Landscape
 *	2: Portrait, flipped
 *	3: Landscape, flipped
 *********************************************************************************
 */

void lcd128x64setOrientation (int orientation)
{
  lcdOrientation = orientation & 3 ;

  lcd128x64setOrigin (0,0) ;

  switch (lcdOrientation)
  {
    case 0:
      maxX = LCD_WIDTH ;
      maxY = LCD_HEIGHT ;
      break ;

    case 1:
      maxX = LCD_HEIGHT ;
      maxY = LCD_WIDTH ;
      break ;

    case 2:
      maxX = LCD_WIDTH ;
      maxY = LCD_HEIGHT ;
      break ;

    case 3:
      maxX = LCD_HEIGHT ;
      maxY = LCD_WIDTH ;
      break ;
  }
}


/*
 * lcd128x64orientCoordinates:
 *	Adjust the coordinates given to the display orientation
 *********************************************************************************
 */

void lcd128x64orientCoordinates (int *x, int *y)
{
  register int tmp ;

  *x += xOrigin ;
  *y += yOrigin ;
  *y  = maxY - *y - 1 ;

  switch (lcdOrientation)
  {
    case 0:
      break;

    case 1:
      tmp = maxY - *y - 1 ;
      *y = *x ;
      *x = tmp ;
      break;

    case 2:
      *x = maxX - *x - 1 ;
      *y = maxY - *y - 1 ;
      break;

    case 3:
      *x = maxX - *x - 1 ;
      tmp = *y ;
      *y = *x ;
      *x = tmp ;
      break ;
  }
}


/*
 * lcd128x64getScreenSize:
 *	Return the max X & Y screen sizes. Needs to be called again, if you 
 *	change screen orientation.
 *********************************************************************************
 */

void lcd128x64getScreenSize (int *x, int *y)
{
  *x = maxX ;
  *y = maxY ;
}


/*
 *********************************************************************************
 * Standard Graphical Functions
 *********************************************************************************
 */


/*
 * lcd128x64point:
 *	Plot a pixel.
 *********************************************************************************
 */

void lcd128x64point (int x, int y, int colour)
{
  lastX = x ;
  lastY = y ;

  lcd128x64orientCoordinates (&x, &y) ;

  if ((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT))
    return ;

  frameBuffer [x + y * LCD_WIDTH] = colour ;
}


/*
 * lcd128x64line: lcd128x64lineTo:
 *	Classic Bressenham Line code
 *********************************************************************************
 */

void lcd128x64line (int x0, int y0, int x1, int y1, int colour)
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
    lcd128x64point (x0, y0, colour) ;

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

void lcd128x64lineTo (int x, int y, int colour)
{
  lcd128x64line (lastX, lastY, x, y, colour) ;
}


/*
 * lcd128x64rectangle:
 *	A rectangle is a spoilt days fishing
 *********************************************************************************
 */

void lcd128x64rectangle (int x1, int y1, int x2, int y2, int colour, int filled)
{
  register int x ;

  if (filled)
  {
    /**/ if (x1 == x2)
      lcd128x64line (x1, y1, x2, y2, colour) ;
    else if (x1 < x2)
      for (x = x1 ; x <= x2 ; ++x)
	lcd128x64line (x, y1, x, y2, colour) ;
    else
      for (x = x2 ; x <= x1 ; ++x)
	lcd128x64line (x, y1, x, y2, colour) ;
  }
  else
  {
    lcd128x64line   (x1, y1, x2, y1, colour) ;
    lcd128x64lineTo (x2, y2, colour) ;
    lcd128x64lineTo (x1, y2, colour) ;
    lcd128x64lineTo (x1, y1, colour) ;
  }
}


/*
 * lcd128x64circle:
 *      This is the midpoint circle algorithm.
 *********************************************************************************
 */

void lcd128x64circle (int x, int y, int r, int colour, int filled)
{
  int ddF_x = 1 ;
  int ddF_y = -2 * r ;

  int f = 1 - r ;
  int x1 = 0 ;
  int y1 = r ;

  if (filled)
  {
    lcd128x64line (x, y + r, x, y - r, colour) ;
    lcd128x64line (x + r, y, x - r, y, colour) ;
  }
  else
  {
    lcd128x64point (x, y + r, colour) ;
    lcd128x64point (x, y - r, colour) ;
    lcd128x64point (x + r, y, colour) ;
    lcd128x64point (x - r, y, colour) ;
  }

  while (x1 < y1)
  {
    if (f >= 0)
    {
      y1-- ;
      ddF_y += 2 ;
      f += ddF_y ;
    }
    x1++ ;
    ddF_x += 2 ;
    f += ddF_x ;
    if (filled)
    {
      lcd128x64line (x + x1, y + y1, x - x1, y + y1, colour) ;
      lcd128x64line (x + x1, y - y1, x - x1, y - y1, colour) ;
      lcd128x64line (x + y1, y + x1, x - y1, y + x1, colour) ;
      lcd128x64line (x + y1, y - x1, x - y1, y - x1, colour) ;
    }
    else
    {
      lcd128x64point (x + x1, y + y1, colour) ; lcd128x64point (x - x1, y + y1, colour) ;
      lcd128x64point (x + x1, y - y1, colour) ; lcd128x64point (x - x1, y - y1, colour) ;
      lcd128x64point (x + y1, y + x1, colour) ; lcd128x64point (x - y1, y + x1, colour) ;
      lcd128x64point (x + y1, y - x1, colour) ; lcd128x64point (x - y1, y - x1, colour) ;
    }
  }
}


/*
 * lcd128x64ellipse:
 *	Fast ellipse drawing algorithm by 
 *      John Kennedy
 *	Mathematics Department
 *	Santa Monica College
 *	1900 Pico Blvd.
 *	Santa Monica, CA 90405
 *	jrkennedy6@gmail.com
 *	-Confirned in email this algorithm is in the public domain -GH-
 *********************************************************************************
 */

static void plot4ellipsePoints (int cx, int cy, int x, int y, int colour, int filled)
{
  if (filled)
  {
    lcd128x64line (cx + x, cy + y, cx - x, cy + y, colour) ;
    lcd128x64line (cx - x, cy - y, cx + x, cy - y, colour) ;
  }
  else
  {
    lcd128x64point (cx + x, cy + y, colour) ;
    lcd128x64point (cx - x, cy + y, colour) ;
    lcd128x64point (cx - x, cy - y, colour) ;
    lcd128x64point (cx + x, cy - y, colour) ;
  }
}

void lcd128x64ellipse (int cx, int cy, int xRadius, int yRadius, int colour, int filled)
{
  int x, y ;
  int xChange, yChange, ellipseError ;
  int twoAsquare, twoBsquare ;
  int stoppingX, stoppingY ;

  twoAsquare = 2 * xRadius * xRadius ;
  twoBsquare = 2 * yRadius * yRadius ;

  x = xRadius ;
  y = 0 ;

  xChange = yRadius * yRadius * (1 - 2 * xRadius) ;
  yChange = xRadius * xRadius ;

  ellipseError = 0 ;
  stoppingX    = twoBsquare * xRadius ;
  stoppingY    = 0 ;

  while (stoppingX >= stoppingY)	// 1st set of points
  {
    plot4ellipsePoints (cx, cy, x, y, colour, filled) ;
    ++y ;
    stoppingY    += twoAsquare ;
    ellipseError += yChange ;
    yChange      += twoAsquare ;

    if ((2 * ellipseError + xChange) > 0 )
    {
      --x ;
      stoppingX    -= twoBsquare ;
      ellipseError += xChange ;
      xChange      += twoBsquare ;
    }
  }

  x = 0 ;
  y = yRadius ;

  xChange = yRadius * yRadius ;
  yChange = xRadius * xRadius * (1 - 2 * yRadius) ;

  ellipseError = 0 ;
  stoppingX    = 0 ;
  stoppingY    = twoAsquare * yRadius ;

  while (stoppingX <= stoppingY)	//2nd set of points
  {
    plot4ellipsePoints (cx, cy, x, y, colour, filled) ;
    ++x ;
    stoppingX    += twoBsquare ;
    ellipseError += xChange ;
    xChange      += twoBsquare ;

    if ((2 * ellipseError + yChange) > 0 )
    {
      --y ;
      stoppingY -= twoAsquare ;
      ellipseError += yChange ;
      yChange += twoAsquare ;
    }
  }
}


/*
 * lcd128x64putchar:
 *	Print a single character to the screen
 *********************************************************************************
 */

void lcd128x64putchar (int x, int y, int c, int bgCol, int fgCol)
{
  int y1, y2 ;

  unsigned char line ;
  unsigned char *fontPtr ;

// Can't print if we're offscreen

//if ((x < 0) || (x >= (maxX - fontWidth)) || (y < 0) || (y >= (maxY - fontHeight)))
//  return ;

  fontPtr = font + c * fontHeight ;

  for (y1 = fontHeight - 1 ; y1 >= 0 ; --y1)
  {
    y2 = y + y1 ;
    line = *fontPtr++ ;
    lcd128x64point (x + 0, y2, (line & 0x80) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 1, y2, (line & 0x40) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 2, y2, (line & 0x20) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 3, y2, (line & 0x10) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 4, y2, (line & 0x08) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 5, y2, (line & 0x04) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 6, y2, (line & 0x02) == 0 ? bgCol : fgCol) ;
    lcd128x64point (x + 7, y2, (line & 0x01) == 0 ? bgCol : fgCol) ;
  }
}


/*
 * lcd128x64puts:
 *	Send a string to the display. Obeys \n and \r formatting
 *********************************************************************************
 */

void lcd128x64puts (int x, int y, const char *str, int bgCol, int fgCol)
{
  int c, mx, my ;

  mx = x ; my = y ;

  while (*str)
  {
    c = *str++ ;

    if (c == '\r')
    {
      mx = x ;
      continue ;
    }

    if (c == '\n')
    {
      mx  = x ;
      my -= fontHeight ;
      continue ;
    }

    lcd128x64putchar (mx, my, c, bgCol, fgCol) ;

    mx += fontWidth ;
    if (mx >= (maxX - fontWidth))
    {
      mx  = 0 ;
      my -= fontHeight ;
    }
  }
}


/*
 * lcd128x64clear:
 *	Clear the display to the given colour.
 *********************************************************************************
 */

void lcd128x64clear (int colour)
{
  register int i ;
  register unsigned char *ptr = frameBuffer ;

  for (i = 0 ; i < (maxX * maxY) ; ++i)
    *ptr++ = colour ;
}




/*
 * lcd128x64setup:
 *	Initialise the display and GPIO.
 *********************************************************************************
 */

int lcd128x64setup (void)
{
  int i ;

  for (i = 0 ; i < 8 ; ++i)
    pinMode (i, OUTPUT) ;

  digitalWrite (CS1,    1) ;
  digitalWrite (CS2,    1) ;
  digitalWrite (STROBE, 0) ;
  digitalWrite (RS,     1) ;

  pinMode (CS1,    OUTPUT) ;
  pinMode (CS2,    OUTPUT) ;
  pinMode (STROBE, OUTPUT) ;
  pinMode (RS,     OUTPUT) ;

  sendCommand (0x3F, CS1) ;	// Display ON
  sendCommand (0xC0, CS1) ;	// Set display start line to 0

  sendCommand (0x3F, CS2) ;	// Display ON
  sendCommand (0xC0, CS2) ;	// Set display start line to 0

  lcd128x64clear          (0) ;
  lcd128x64setOrientation (0) ;
  lcd128x64update         () ;

  return 0 ;
}
