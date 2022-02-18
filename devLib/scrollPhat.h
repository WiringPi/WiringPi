/*
 * scrollPhat.h:
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

extern void scrollPhatPoint      (int x, int y, int colour) ;
extern void scrollPhatLine       (int x0, int y0, int x1, int y1, int colour) ;
extern void scrollPhatLineTo     (int x, int y, int colour) ;
extern void scrollPhatRectangle  (int x1, int y1, int x2, int y2, int colour, int filled) ;
extern void scrollPhatUpdate     (void) ;
extern void scrollPhatClear      (void) ;

extern int  scrollPhatPutchar    (int c) ;
//extern void scrollPhatPutchar    (int c) ;
extern void scrollPhatPuts       (const char *str) ;
extern void scrollPhatPrintf     (const char *message, ...) ;
extern void scrollPhatPrintSpeed (const int cps10) ;

extern void scrollPhatIntensity  (const int percent) ;
extern int  scrollPhatSetup      (void) ;
