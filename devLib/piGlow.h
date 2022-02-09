/*
 * piglow.h:
 *	Easy access to the Pimoroni PiGlow board.
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


#define	PIGLOW_RED	0
#define	PIGLOW_ORANGE	1
#define	PIGLOW_YELLOW	2
#define	PIGLOW_GREEN	3
#define	PIGLOW_BLUE	4
#define	PIGLOW_WHITE	5


#ifdef __cplusplus
extern "C" {
#endif

extern void piGlow1     (const int leg,  const int ring, const int intensity) ;
extern void piGlowLeg   (const int leg,  const int intensity) ;
extern void piGlowRing  (const int ring, const int intensity) ;
extern void piGlowSetup (int clear) ;

#ifdef __cplusplus
}
#endif
