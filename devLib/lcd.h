/*
 * lcd.h:
 *	Text-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based in the Hitachi HD44780U controller and compatables.
 *
 * Copyright (c) 2012 Gordon Henderson.
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

#define	MAX_LCDS	8

#ifdef __cplusplus
extern "C" {
#endif

extern void lcdHome        (const int fd) ;
extern void lcdClear       (const int fd) ;
extern void lcdDisplay     (const int fd, int state) ;
extern void lcdCursor      (const int fd, int state) ;
extern void lcdCursorBlink (const int fd, int state) ;
extern void lcdSendCommand (const int fd, unsigned char command) ;
extern void lcdPosition    (const int fd, int x, int y) ;
extern void lcdCharDef     (const int fd, int index, unsigned char data [8]) ;
extern void lcdPutchar     (const int fd, unsigned char data) ;
extern void lcdPuts        (const int fd, const char *string) ;
extern void lcdPrintf      (const int fd, const char *message, ...) ;

extern int  lcdInit (const int rows, const int cols, const int bits,
	const int rs, const int strb,
	const int d0, const int d1, const int d2, const int d3, const int d4,
	const int d5, const int d6, const int d7) ;

#ifdef __cplusplus
}
#endif
