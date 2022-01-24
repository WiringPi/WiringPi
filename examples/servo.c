/*
 * servo.c:
 *	Test of the softServo code.
 *	Do not use this code - use the servoBlaster kernel module instead
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/WiringPi/WiringPi
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
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <softServo.h>

int main ()
{
  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "oops: %s\n", strerror (errno)) ;
    return 1 ;
  }

  softServoSetup (0, 1, 2, 3, 4, 5, 6, 7) ;

  softServoWrite (0,  0) ;
/*
  softServoWrite (1, 1000) ;
  softServoWrite (2, 1100) ;
  softServoWrite (3, 1200) ;
  softServoWrite (4, 1300) ;
  softServoWrite (5, 1400) ;
  softServoWrite (6, 1500) ;
  softServoWrite (7, 2200) ;
*/

  for (;;)
    delay (10) ;

}
