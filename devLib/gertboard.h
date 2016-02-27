/*
 * gertboard.h:
 *	Access routines for the SPI devices on the Gertboard
 *	Copyright (c) 2012 Gordon Henderson
 *
 *	The Gertboard has an MCP4802 dual-channel D to A convertor
 *	connected to the SPI bus, selected via chip-select B.
 *
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifdef __cplusplus
extern "C" {
#endif

// Old routines

extern void gertboardAnalogWrite (const int chan, const int value) ;
extern int  gertboardAnalogRead  (const int chan) ;
extern int  gertboardSPISetup    (void) ;

// New

extern int  gertboardAnalogSetup (const int pinBase) ;

#ifdef __cplusplus
}
#endif
