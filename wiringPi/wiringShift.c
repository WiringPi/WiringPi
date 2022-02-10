/*
 * wiringShift.c:
 *	Emulate some of the Arduino wiring functionality. 
 *
 * Copyright (c) 2009-2012 Gordon Henderson.
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

#include <stdint.h>

#include "wiringPi.h"
#include "wiringShift.h"

/*
 * shiftIn:
 *	Shift data in from a clocked source
 *********************************************************************************
 */

uint8_t shiftIn (uint8_t dPin, uint8_t cPin, uint8_t order)
{
  uint8_t value = 0 ;
  int8_t  i ;
 
  if (order == MSBFIRST)
    for (i = 7 ; i >= 0 ; --i)
    {
      digitalWrite (cPin, HIGH) ;
      value |= digitalRead (dPin) << i ;
      digitalWrite (cPin, LOW) ;
    }
  else
    for (i = 0 ; i < 8 ; ++i)
    {
      digitalWrite (cPin, HIGH) ;
      value |= digitalRead (dPin) << i ;
      digitalWrite (cPin, LOW) ;
    }

  return value;
}

/*
 * shiftOut:
 *	Shift data out to a clocked source
 *********************************************************************************
 */

void shiftOut (uint8_t dPin, uint8_t cPin, uint8_t order, uint8_t val)
{
  int8_t i;

  if (order == MSBFIRST)
    for (i = 7 ; i >= 0 ; --i)
    {
      digitalWrite (dPin, val & (1 << i)) ;
      digitalWrite (cPin, HIGH) ;
      digitalWrite (cPin, LOW) ;
    }
  else
    for (i = 0 ; i < 8 ; ++i)
    {
      digitalWrite (dPin, val & (1 << i)) ;
      digitalWrite (cPin, HIGH) ;
      digitalWrite (cPin, LOW) ;
    }
}
