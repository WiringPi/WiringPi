/*
 * ds1302.c:
 *	Real Time clock
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>

#include <wiringPi.h>

#include "ds1302.h"

// Register defines

#define	RTC_SECS	 0
#define	RTC_MINS	 1
#define	RTC_HOURS	 2
#define	RTC_DATE	 3
#define	RTC_MONTH	 4
#define	RTC_DAY		 5
#define	RTC_YEAR	 6
#define	RTC_WP		 7
#define	RTC_TC		 8
#define	RTC_BM		31


// Locals

static int dPin, cPin, sPin ;

/*
 * dsShiftIn:
 *	Shift a number in from the chip, LSB first. Note that the data is
 *	sampled on the trailing edge of the last clock, so it's valid immediately.
 *********************************************************************************
 */

static unsigned int dsShiftIn (void)
{
  uint8_t value = 0 ;
  int i ;

  pinMode (dPin, INPUT) ;	delayMicroseconds (1) ;

  for (i = 0 ; i < 8 ; ++i)
  {
    value |= (digitalRead (dPin) << i) ;
    digitalWrite (cPin, HIGH) ; delayMicroseconds (1) ;
    digitalWrite (cPin, LOW) ;	delayMicroseconds (1) ;
  }

  return value;
}


/*
 * dsShiftOut:
 *	A normal LSB-first shift-out, just slowed down a bit - the Pi is
 *	a bit faster than the chip can handle.
 *********************************************************************************
 */

static void dsShiftOut (unsigned int data)
{
  int i ;

  pinMode (dPin, OUTPUT) ;

  for (i = 0 ; i < 8 ; ++i)
  {
    digitalWrite (dPin, data & (1 << i)) ;	delayMicroseconds (1) ;
    digitalWrite (cPin, HIGH) ;			delayMicroseconds (1) ;
    digitalWrite (cPin, LOW) ;			delayMicroseconds (1) ;
  }
}


/*
 * ds1302regRead: ds1302regWrite:
 *	Read/Write a value to an RTC Register or RAM location on the chip
 *********************************************************************************
 */

static unsigned int ds1302regRead (const int reg)
{
  unsigned int data ;

  digitalWrite (sPin, HIGH) ; delayMicroseconds (1) ;
    dsShiftOut (reg) ;
    data = dsShiftIn () ;
  digitalWrite (sPin, LOW)  ; delayMicroseconds (1) ;

  return data ;
}

static void ds1302regWrite (const int reg, const unsigned int data)
{
  digitalWrite (sPin, HIGH) ; delayMicroseconds (1) ;
    dsShiftOut (reg) ;
    dsShiftOut (data) ;
  digitalWrite (sPin, LOW)  ; delayMicroseconds (1) ;
}


/*
 * ds1302rtcWrite: ds1302rtcRead:
 *	Writes/Reads the data to/from the RTC register
 *********************************************************************************
 */

unsigned int ds1302rtcRead (const int reg)
{
  return ds1302regRead (0x81 | ((reg & 0x1F) << 1)) ;
}

void ds1302rtcWrite (int reg, unsigned int data)
{
  ds1302regWrite (0x80 | ((reg & 0x1F) << 1), data) ;
}


/*
 * ds1302ramWrite: ds1302ramRead:
 *	Writes/Reads the data to/from the RTC register
 *********************************************************************************
 */

unsigned int ds1302ramRead (const int addr)
{
  return ds1302regRead (0xC1 | ((addr & 0x1F) << 1)) ;
}

void ds1302ramWrite (const int addr, const unsigned int data)
{
  ds1302regWrite ( 0xC0 | ((addr & 0x1F) << 1), data) ;
}

/*
 * ds1302clockRead:
 *	Read all 8 bytes of the clock in a single operation
 *********************************************************************************
 */

void ds1302clockRead (int clockData [8])
{
  int i ;
  unsigned int regVal = 0x81 | ((RTC_BM & 0x1F) << 1) ;

  digitalWrite (sPin, HIGH) ; delayMicroseconds (1) ;

  dsShiftOut (regVal) ;
  for (i = 0 ; i < 8 ; ++i)
    clockData [i] = dsShiftIn () ;

  digitalWrite (sPin, LOW) ;  delayMicroseconds (1) ;
}


/*
 * ds1302clockWrite:
 *	Write all 8 bytes of the clock in a single operation
 *********************************************************************************
 */

void ds1302clockWrite (const int clockData [8])
{
  int i ;
  unsigned int regVal = 0x80 | ((RTC_BM & 0x1F) << 1) ;

  digitalWrite (sPin, HIGH) ; delayMicroseconds (1) ;

  dsShiftOut (regVal) ;
  for (i = 0 ; i < 8 ; ++i)
    dsShiftOut (clockData [i]) ;

  digitalWrite (sPin, LOW) ;  delayMicroseconds (1) ;
}


/*
 * ds1302trickleCharge:
 *	Set the bits on the trickle charger.
 *	Probably best left alone...
 *********************************************************************************
 */

void ds1302trickleCharge (const int diodes, const int resistors)
{
  if (diodes + resistors == 0)
    ds1302rtcWrite (RTC_TC, 0x5C) ;	// Disabled
  else
    ds1302rtcWrite (RTC_TC, 0xA0 | ((diodes & 3) << 2) | (resistors & 3)) ;
}




/*
 * ds1302setup:
 *	Initialise the chip & remember the pins we're using
 *********************************************************************************
 */

void ds1302setup (const int clockPin, const int dataPin, const int csPin)
{
  dPin = dataPin ;
  cPin = clockPin ;
  sPin = csPin ;

  digitalWrite (dPin, LOW) ;
  digitalWrite (cPin, LOW) ;
  digitalWrite (sPin, LOW) ;

  pinMode (dPin, OUTPUT) ;
  pinMode (cPin, OUTPUT) ;
  pinMode (sPin, OUTPUT) ;

  ds1302rtcWrite (RTC_WP, 0) ;	// Remove write-protect
}
