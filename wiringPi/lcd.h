/*
 * lcd.h:
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

#define	MAX_LCDS	8

#ifdef __cplusplus
extern "C" {
#endif

extern void lcdHome        (int fd) ;
extern void lcdClear       (int fd) ;
extern void lcdSendCommand (int fd, uint8_t command) ;
extern void lcdPosition    (int fd, int x, int y) ;
extern void lcdPutchar     (int fd, uint8_t data) ;
extern void lcdPuts        (int fd, char *string) ;
extern void lcdPrintf      (int fd, char *message, ...) ;

extern int  lcdInit (int rows, int cols, int bits, int rs, int strb,
	int d0, int d1, int d2, int d3, int d4, int d5, int d6, int d7) ;

#ifdef __cplusplus
}
#endif
