/*
 * lcd.c:
 *	Text-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based in the Hitachi HD44780U controller and compatables.
 *
 *	This test program assumes the following:
 *
 *	8-bit displays:
 *		GPIO 0-7 is connected to display data pins 0-7.
 *		GPIO 11 is the RS pin.
 *		GPIO 10 is the Strobe/E pin.
 *
 *	For 4-bit interface:
 *		GPIO 4-7 is connected to display data pins 4-7.
 *		GPIO 11 is the RS pin.
 *		GPIO 10 is the Strobe/E pin.
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
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
#include <stdint.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

static unsigned char newChar [8] = 
{
  0b11111,
  0b10001,
  0b10001,
  0b10101,
  0b11111,
  0b10001,
  0b10001,
  0b11111,
} ;


// Global lcd handle:

static int lcdHandle ;

/*
 * usage:
 *********************************************************************************
 */

int usage (const char *progName)
{
  fprintf (stderr, "Usage: %s bits cols rows\n", progName) ;
  return EXIT_FAILURE ;
}


/*
 * scrollMessage:
 *********************************************************************************
 */

static const char *message =
  "                    "
  "Wiring Pi by Gordon Henderson. HTTP://WIRINGPI.COM/"
  "                    " ;

void scrollMessage (int line, int width)
{
  char buf [32] ;
  static int position = 0 ;
  static int timer = 0 ;

  if (millis () < timer)
    return ;

  timer = millis () + 200 ;

  strncpy (buf, &message [position], width) ;
  buf [width] = 0 ;
  lcdPosition (lcdHandle, 0, line) ;
  lcdPuts     (lcdHandle, buf) ;

  if (++position == (strlen (message) - width))
    position = 0 ;
}


/*
 * pingPong:
 *	Bounce a character - only on 4-line displays
 *********************************************************************************
 */

static void pingPong (int lcd, int cols)
{
  static int position = 0 ;
  static int dir      = 0 ;

  if (dir == 0)		// Setup
  {
    dir = 1 ;
    lcdPosition (lcdHandle, 0, 3) ;
    lcdPutchar  (lcdHandle, '*') ;
    return ;
  }

  lcdPosition (lcdHandle, position, 3) ;
  lcdPutchar (lcdHandle, ' ') ;
  position += dir ;

  if (position == cols)
  {
    dir = -1 ;
    --position ;
  }
  
  if (position < 0)
  {
    dir = 1 ;
    ++position ;
  }

  lcdPosition (lcdHandle, position, 3) ;
  lcdPutchar  (lcdHandle, '#') ;
}


/*
 * waitForEnter:
 *********************************************************************************
 */

static void waitForEnter (void)
{
  printf ("Press ENTER to continue: ") ;
  (void)fgetc (stdin) ;
}


/*
 * The works
 *********************************************************************************
 */

int main (int argc, char *argv[])
{
  int i ;
  int lcd ;
  int bits, rows, cols ;

  struct tm *t ;
  time_t tim ;

  char buf [32] ;

  if (argc != 4)
    return usage (argv [0]) ;

  printf ("Raspberry Pi LCD test\n") ;
  printf ("=====================\n") ;

  bits = atoi (argv [1]) ;
  cols = atoi (argv [2]) ;
  rows = atoi (argv [3]) ;

  if (!((rows == 1) || (rows == 2) || (rows == 4)))
  {
    fprintf (stderr, "%s: rows must be 1, 2 or 4\n", argv [0]) ;
    return EXIT_FAILURE ;
  }

  if (!((cols == 16) || (cols == 20)))
  {
    fprintf (stderr, "%s: cols must be 16 or 20\n", argv [0]) ;
    return EXIT_FAILURE ;
  }

  wiringPiSetup () ;

  if (bits == 4)
    lcdHandle = lcdInit (rows, cols, 4, 11,10, 4,5,6,7,0,0,0,0) ;
  else
    lcdHandle = lcdInit (rows, cols, 8, 11,10, 0,1,2,3,4,5,6,7) ;

  if (lcdHandle < 0)
  {
    fprintf (stderr, "%s: lcdInit failed\n", argv [0]) ;
    return -1 ;
  }

  lcdPosition (lcdHandle, 0, 0) ; lcdPuts (lcdHandle, "Hello World") ;
  lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, "  wiringpi  ") ;

  waitForEnter () ;

  if (rows > 1)
  {
    lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, "  wiringpi  ") ;

    if (rows == 4)
    {
      lcdPosition (lcdHandle, 0, 2) ;
      for (i = 0 ; i < ((cols - 1) / 2) ; ++i)
        lcdPuts (lcdHandle, "=-") ;
      lcdPuts (lcdHandle, "=3") ;

      lcdPosition (lcdHandle, 0, 3) ;
      for (i = 0 ; i < ((cols - 1) / 2) ; ++i)
        lcdPuts (lcdHandle, "-=") ;
      lcdPuts (lcdHandle, "-4") ;
    }
  }

  waitForEnter () ;

  lcdCharDef  (lcdHandle, 2, newChar) ;

  lcdClear    (lcdHandle) ;
  lcdPosition (lcdHandle, 0, 0) ;
  lcdPuts     (lcdHandle, "User Char: ") ;
  lcdPutchar  (lcdHandle, 2) ;

  lcdCursor      (lcdHandle, TRUE) ;
  lcdCursorBlink (lcdHandle, TRUE) ;

  waitForEnter () ;

  lcdCursor      (lcdHandle, FALSE) ;
  lcdCursorBlink (lcdHandle, FALSE) ;
  lcdClear       (lcdHandle) ;

  for (;;)
  {
    scrollMessage (0, cols) ;
    
    if (rows == 1)
      continue ;

    tim = time (NULL) ;
    t = localtime (&tim) ;

    sprintf (buf, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec) ;

    lcdPosition (lcdHandle, (cols - 8) / 2, 1) ;
    lcdPuts     (lcdHandle, buf) ;

    if (rows == 2)
      continue ;

    sprintf (buf, "%02d/%02d/%04d", t->tm_mday, t->tm_mon + 1, t->tm_year+1900) ;

    lcdPosition (lcdHandle, (cols - 10) / 2, 2) ;
    lcdPuts     (lcdHandle, buf) ;

    pingPong (lcd, cols) ;
  }

  return 0 ;
}
