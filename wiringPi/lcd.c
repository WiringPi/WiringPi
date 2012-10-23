/*
 * lcd.c:
 *	Text-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based in the Hitachi HD44780U controller and compatables.
 *
 * Copyright (c) 2012 Gordon Henderson.
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
#include <stdarg.h>

#include "wiringPi.h"
#include "lcd.h"

// Commands

#define	LCD_CLEAR	0x01
#define	LCD_HOME	0x02
#define	LCD_ENTRY	0x04
#define	LCD_ON_OFF	0x08
#define	LCD_CDSHIFT	0x10
#define	LCD_FUNC	0x20
#define	LCD_CGRAM	0x40
#define	LCD_DGRAM	0x80

#define	LCD_ENTRY_SH	0x01
#define	LCD_ENTRY_ID	0x02

#define	LCD_ON_OFF_B	0x01
#define	LCD_ON_OFF_C	0x02
#define	LCD_ON_OFF_D	0x04

#define	LCD_FUNC_F	0x04
#define	LCD_FUNC_N	0x08
#define	LCD_FUNC_DL	0x10

#define	LCD_CDSHIFT_RL	0x04

struct lcdDataStruct
{
  uint8_t bits, rows, cols ;
  uint8_t rsPin, strbPin ;
  uint8_t dataPins [8] ;
} ;

struct lcdDataStruct *lcds [MAX_LCDS] ;


/*
 * strobe:
 *	Toggle the strobe (Really the "E") pin to the device.
 *	According to the docs, data is latched on the falling edge.
 *********************************************************************************
 */

static void strobe (struct lcdDataStruct *lcd)
{

// Note timing changes for new version of delayMicroseconds ()

  digitalWrite (lcd->strbPin, 1) ; delayMicroseconds (50) ;
  digitalWrite (lcd->strbPin, 0) ; delayMicroseconds (50) ;
}


/*
 * sentDataCmd:
 *	Send an data or command byte to the display.
 *********************************************************************************
 */

static void sendDataCmd (struct lcdDataStruct *lcd, uint8_t data)
{
  uint8_t i, d4 ;

  if (lcd->bits == 4)
  {
    d4 = (data >> 4) & 0x0F;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
    strobe (lcd) ;

    d4 = data & 0x0F ;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
  }
  else
  {
    for (i = 0 ; i < 8 ; ++i)
    {
      digitalWrite (lcd->dataPins [i], (data & 1)) ;
      data >>= 1 ;
    }
  }
  strobe (lcd) ;
}


/*
 * putCommand:
 *	Send a command byte to the display
 *********************************************************************************
 */

static void putCommand (struct lcdDataStruct *lcd, uint8_t command)
{
  digitalWrite (lcd->rsPin,   0) ;
  sendDataCmd  (lcd, command) ;
}

static void put4Command (struct lcdDataStruct *lcd, uint8_t command)
{
  uint8_t i ;

  digitalWrite (lcd->rsPin,   0) ;

  for (i = 0 ; i < 4 ; ++i)
  {
    digitalWrite (lcd->dataPins [i], (command & 1)) ;
    command >>= 1 ;
  }
  strobe (lcd) ;
}


/*
 *********************************************************************************
 * User Code below here
 *********************************************************************************
 */

/*
 * lcdHome: lcdClear:
 *	Home the cursor or clear the screen.
 *********************************************************************************
 */

void lcdHome (int fd)
{
  struct lcdDataStruct *lcd = lcds [fd] ;
  putCommand (lcd, LCD_HOME) ;
}

void lcdClear (int fd)
{
  struct lcdDataStruct *lcd = lcds [fd] ;
  putCommand (lcd, LCD_CLEAR) ;
}


/*
 * lcdPosition:
 *	Update the position of the cursor on the display
 *********************************************************************************
 */


void lcdPosition (int fd, int x, int y)
{
  static uint8_t rowOff [4] = { 0x00, 0x40, 0x14, 0x54 } ;
  struct lcdDataStruct *lcd = lcds [fd] ;

  putCommand (lcd, x + (LCD_DGRAM | rowOff [y])) ;
}


/*
 * lcdPutchar:
 *	Send a data byte to be displayed on the display
 *********************************************************************************
 */

void lcdPutchar (int fd, uint8_t data)
{
  struct lcdDataStruct *lcd = lcds [fd] ;

  digitalWrite (lcd->rsPin, 1) ;
  sendDataCmd (lcd, data) ;
}


/*
 * lcdPuts:
 *	Send a string to be displayed on the display
 *********************************************************************************
 */

void lcdPuts (int fd, char *string)
{
  while (*string)
    lcdPutchar (fd, *string++) ;
}


/*
 * lcdPrintf:
 *	Printf to an LCD display
 *********************************************************************************
 */

void lcdPrintf (int fd, char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  lcdPuts (fd, buffer) ;
}


/*
 * lcdInit:
 *	Take a lot of parameters and initialise the LCD, and return a handle to
 *	that LCD, or -1 if any error.
 *********************************************************************************
 */

int lcdInit (int rows, int cols, int bits, int rs, int strb,
	int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7)
{
  static int initialised = 0 ;

  uint8_t func ;
  int i ;
  int lcdFd = -1 ;
  struct lcdDataStruct *lcd ;

  if (initialised == 0)
  {
    initialised = 1 ;
    for (i = 0 ; i < MAX_LCDS ; ++i)
      lcds [i] = NULL ;
  }

// Simple sanity checks

  if (! ((bits == 4) || (bits == 8)))
    return -1 ;

  if ((rows < 0) || (rows > 20))
    return -1 ;

  if ((cols < 0) || (cols > 20))
    return -1 ;

// Create a new LCD:

  for (i = 0 ; i < MAX_LCDS ; ++i)
  {
    if (lcds [i] == NULL)
    {
      lcdFd = i ;
      break ;
    }
  }

  if (lcdFd == -1)
    return -1 ;

  lcd = malloc (sizeof (struct lcdDataStruct)) ;
  if (lcd == NULL)
    return -1 ;

  lcd->rsPin   = rs ;
  lcd->strbPin = strb ;
  lcd->bits    = 8 ;		// For now - we'll set it properly later.
  lcd->rows    = rows ;
  lcd->cols    = cols ;

  lcd->dataPins [0] = d0 ;
  lcd->dataPins [1] = d1 ;
  lcd->dataPins [2] = d2 ;
  lcd->dataPins [3] = d3 ;
  lcd->dataPins [4] = d4 ;
  lcd->dataPins [5] = d5 ;
  lcd->dataPins [6] = d6 ;
  lcd->dataPins [7] = d7 ;

  lcds [lcdFd] = lcd ;

  digitalWrite (lcd->rsPin,   0) ; pinMode (lcd->rsPin,   OUTPUT) ;
  digitalWrite (lcd->strbPin, 0) ; pinMode (lcd->strbPin, OUTPUT) ;

  for (i = 0 ; i < bits ; ++i)
  {
    digitalWrite (lcd->dataPins [i], 0) ;
    pinMode      (lcd->dataPins [i], OUTPUT) ;
  }
  delay (35) ; // mS


// 4-bit mode?
//	OK. This is a PIG and it's not at all obvious from the documentation I had,
//	so I guess some others have worked through either with better documentation
//	or more trial and error... Anyway here goes:
//
//	It seems that the controller needs to see the FUNC command at least 3 times
//	consecutively - in 8-bit mode. If you're only using 8-bit mode, then it appears
//	that you can get away with one func-set, however I'd not rely on it...
//
//	So to set 4-bit mode, you need to send the commands one nibble at a time,
//	the same three times, but send the command to set it into 8-bit mode those
//	three times, then send a final 4th command to set it into 4-bit mode, and only
//	then can you flip the switch for the rest of the library to work in 4-bit
//	mode which sends the commands as 2 x 4-bit values.

  if (bits == 4)
  {
    func = LCD_FUNC | LCD_FUNC_DL ;			// Set 8-bit mode 3 times
    put4Command (lcd, func >> 4) ; delay (35) ;
    put4Command (lcd, func >> 4) ; delay (35) ;
    put4Command (lcd, func >> 4) ; delay (35) ;
    func = LCD_FUNC ;					// 4th set: 4-bit mode
    put4Command (lcd, func >> 4) ; delay (35) ;
    lcd->bits = 4 ;
  }
  else
  {
    func = LCD_FUNC | LCD_FUNC_DL ;
    putCommand  (lcd, func     ) ; delay (35) ;
    putCommand  (lcd, func     ) ; delay (35) ;
    putCommand  (lcd, func     ) ; delay (35) ;
  }

  if (lcd->rows > 1)
  {
    func |= LCD_FUNC_N ;
    putCommand (lcd, func) ; delay (35) ;
  }

// Rest of the initialisation sequence

  putCommand (lcd, LCD_ON_OFF  | LCD_ON_OFF_D) ;   delay (2) ;
  putCommand (lcd, LCD_ENTRY   | LCD_ENTRY_ID) ;   delay (2) ;
  putCommand (lcd, LCD_CDSHIFT | LCD_CDSHIFT_RL) ; delay (2) ;
  putCommand (lcd, LCD_CLEAR) ;                    delay (5) ;

  return lcdFd ;
}
