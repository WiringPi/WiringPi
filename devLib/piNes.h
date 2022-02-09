/*
 * piNes.h:
 *	Driver for the NES Joystick controller on the Raspberry Pi
 *	Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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

#define	MAX_NES_JOYSTICKS	8

#define	NES_RIGHT	0x01
#define	NES_LEFT	0x02
#define	NES_DOWN	0x04
#define	NES_UP		0x08
#define	NES_START	0x10
#define	NES_SELECT	0x20
#define	NES_B		0x40
#define	NES_A		0x80

#ifdef __cplusplus
extern "C" {
#endif

extern int          setupNesJoystick (int dPin, int cPin, int lPin) ;
extern unsigned int  readNesJoystick (int joystick) ;

#ifdef __cplusplus
}
#endif
