/*
 * lcd128x64.h:
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

extern void lcd128x64setOrigin         (int x, int y) ;
extern void lcd128x64setOrientation    (int orientation) ;
extern void lcd128x64orientCoordinates (int *x, int *y) ;
extern void lcd128x64getScreenSize     (int *x, int *y) ;
extern void lcd128x64point             (int  x, int  y, int colour) ;
extern void lcd128x64line              (int x0, int y0, int x1, int y1, int colour) ;
extern void lcd128x64lineTo            (int  x, int  y, int colour) ;
extern void lcd128x64rectangle         (int x1, int y1, int x2, int y2, int colour, int filled) ;
extern void lcd128x64circle            (int  x, int  y, int  r, int colour, int filled) ;
extern void lcd128x64ellipse           (int cx, int cy, int xRadius, int yRadius, int colour, int filled) ;
extern void lcd128x64putchar           (int  x, int  y, int c, int bgCol, int fgCol) ;
extern void lcd128x64puts              (int  x, int  y, const char *str, int bgCol, int fgCol) ;
extern void lcd128x64update            (void) ;
extern void lcd128x64clear             (int colour) ;

extern int  lcd128x64setup             (void) ;
