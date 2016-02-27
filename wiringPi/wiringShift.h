/*
 * wiringShift.h:
 *	Emulate some of the Arduino wiring functionality. 
 *
 * Copyright (c) 2009-2012 Gordon Henderson.
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

#define	LSBFIRST	0
#define	MSBFIRST	1

#ifndef	_STDINT_H
#  include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t shiftIn      (uint8_t dPin, uint8_t cPin, uint8_t order) ;
extern void    shiftOut     (uint8_t dPin, uint8_t cPin, uint8_t order, uint8_t val) ;

#ifdef __cplusplus
}
#endif
