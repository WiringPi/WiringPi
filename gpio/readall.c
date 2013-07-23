/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2013 Gordon Henderson
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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>

extern int wpMode ;

/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 *********************************************************************************
 */

static void doReadallExternal (void)
{
  int pin ;

  printf ("+------+---------+--------+\n") ;
  printf ("|  Pin | Digital | Analog |\n") ;
  printf ("+------+---------+--------+\n") ;

  for (pin = wiringPiNodes->pinBase ; pin <= wiringPiNodes->pinMax ; ++pin)
    printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin)) ;

  printf ("+------+---------+--------+\n") ;
}


/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 *********************************************************************************
 */

static char *pinNames [] =
{
  "GPIO 0", "GPIO 1", "GPIO 2", "GPIO 3", "GPIO 4", "GPIO 5", "GPIO 6", "GPIO 7",
  "SDA   ", "SCL   ",
  "CE0   ", "CE1   ", "MOSI  ", "MISO  ", "SCLK  ",
  "TxD   ", "RxD   ",
  "GPIO 8", "GPIO 9", "GPIO10", "GPIO11",
} ;

static char *alts [] =
{
  "IN", "OUT", "ALT5", "ALT4", "ALT0", "ALT1", "ALT2", "ALT3"
} ;

static int wpiToPhys [64] =
{
  11, 12, 13, 15, 16, 18, 22,  7,	//  0...7
   3,  5,				//  8...9
  24, 26, 19, 21, 23,			// 10..14
   8, 10,				// 15..16
   3,  4,  5,  6,			// 17..20
             0,0,0,0,0,0,0,0,0,0,0,	// 20..31
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 32..47
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// 47..63
} ;

// The other mappings needed are in wiringPi.c

static int physToWpi [64] = 
{
  -1,           // 0
  -1, -1,       // 1, 2
   8, -1,
   9, -1,
   7, 15,
  -1, 16,
   0,  1,
   2, -1,
   3,  4,
  -1,  5,
  12, -1,
  13,  6,
  14, 10,
  -1, 11,       // 25, 26

// Padding:

                                              -1, -1, -1, -1, -1,       // ... 31
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       // ... 47
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,       // ... 63
} ;

static char *physNames [64] = 
{
  NULL,

  "3.3v",  "5v",
  "SDA",   "5V",
  "SCL",   "0v",
  "GPIO7", "TxD",
  "0v",    "RxD",
  "GPIO0", "GPIO1",
  "GPIO2", "0v",
  "GPIO3", "GPIO4",
  "3.3v",  "GPIO5",
  "MOSI",  "0v",
  "MISO",  "GPIO6",
  "SCLK",  "CE0",
  "0v",    "CE1",

                                                         NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
  NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
} ;

static void readallPhys (int physPin)
{
  int pin ;

  /**/ if (wpMode == WPI_MODE_GPIO)
  {
    if (physPinToGpio (physPin) == -1)
      printf (" |    ") ;
    else
      printf (" | %3d", physPinToGpio (physPin)) ;
  }
  else if (wpMode != WPI_MODE_PHYS)
  {
    if (physToWpi     [physPin] == -1)
      printf (" |    ") ;
    else
      printf (" | %3d", physToWpi     [physPin]) ;
  }

  printf (" | %5s", physNames [physPin]) ;

  if (physToWpi [physPin] == -1)
    printf (" |      |   ") ;
  else
  {
    /**/ if (wpMode == WPI_MODE_GPIO)
      pin = physPinToGpio (physPin) ;
    else if (wpMode == WPI_MODE_PHYS)
      pin = physPin ;
    else
      pin = physToWpi [physPin] ;

    printf (" | %4s", alts [getAlt (pin)]) ;
    printf (" | %s", (digitalRead (pin) == LOW) ? "Lo" : "Hi") ;
  }

// Pin numbers:

  printf (" | %2d", physPin) ;
  ++physPin ;
  printf (" || %-2d", physPin) ;

// Same, reversed

  if (physToWpi [physPin] == -1)
    printf (" |    |     ") ;
  else
  {
    /**/ if (wpMode == WPI_MODE_GPIO)
      pin = physPinToGpio (physPin) ;
    else if (wpMode == WPI_MODE_PHYS)
      pin = physPin ;
    else
      pin = physToWpi [physPin] ;

    printf (" | %s", (digitalRead (pin) == LOW) ? "Lo" : "Hi") ;
    printf (" | %-4s", alts [getAlt (pin)]) ;
  }

  printf (" | %-5s", physNames [physPin]) ;

  /**/ if (wpMode == WPI_MODE_GPIO)
  {
    if (physPinToGpio (physPin) == -1)
      printf (" |    ") ;
    else
      printf (" | %-3d", physPinToGpio (physPin)) ;
  }
  else if (wpMode != WPI_MODE_PHYS)
  {
    if (physToWpi     [physPin] == -1)
      printf (" |    ") ;
    else
      printf (" | %-3d", physToWpi     [physPin]) ;
  }

  printf (" |\n") ;
}


void doReadall (void)
{
  int pin ;

  if (wiringPiNodes != NULL)	// External readall
  {
    doReadallExternal () ;
    return ;
  }

  /**/ if (wpMode == WPI_MODE_GPIO)
  {
    printf (" +-----+-------+------+----+-Rev%d-----+----+------+-------+-----+\n", piBoardRev ()) ;
    printf (" | BCM |  Name | Mode | Val| Physical |Val | Mode | Name  | BCM |\n") ;
    printf (" +-----+-------+------+----+----++----+----+------+-------+-----+\n") ;
    for (pin = 1 ; pin <= 26 ; pin += 2)
      readallPhys (pin) ;
    printf (" +-----+-------+------+----+----++----+----+------+-------+-----+\n") ;
  }
  else if (wpMode == WPI_MODE_PHYS)
  {
    printf (" +-------+------+----+-Rev%d-----+----+------+-------+\n", piBoardRev ()) ;
    printf (" |  Name | Mode | Val| Physical |Val | Mode | Name  |\n") ;
    printf (" +-------+------+----+----++----+----+------+-------+\n") ;
    for (pin = 1 ; pin <= 26 ; pin += 2)
      readallPhys (pin) ;
    printf (" +-------+------+----+----++----+----+------+-------+\n") ;
  }
  else	// wiringPi
  {
    printf (" +-----+-------+------+----+-Rev%d-----+----+------+-------+-----+\n", piBoardRev ()) ;
    printf (" | wPi |  Name | Mode | Val| Physical |Val | Mode | Name  | wPi |\n") ;
    printf (" +-----+-------+------+----+----++----+----+------+-------+-----+\n") ;
    for (pin = 1 ; pin <= 26 ; pin += 2)
      readallPhys (pin) ;
    printf (" +-----+-------+------+----+----++----+----+------+-------+-----+\n") ;
  }
}


void doReadallOld (void)
{
  int pin ;

  if (wiringPiNodes != NULL)	// External readall
  {
    doReadallExternal () ;
    return ;
  }

  printf ("+----------+-Rev%d-+------+--------+------+-------+\n", piBoardRev ()) ;
  printf ("| wiringPi | GPIO | Phys | Name   | Mode | Value |\n") ;
  printf ("+----------+------+------+--------+------+-------+\n") ;

  for (pin = 0 ; pin < 64 ; ++pin)	// Crude, but effective
  {
    if (wpiPinToGpio (pin) == -1)
      continue ;

    printf ("| %6d   | %3d  | %3d  | %s | %-4s | %-4s  |\n",
	pin, wpiPinToGpio (pin), wpiToPhys [pin],
	pinNames [pin], 
	alts [getAlt (pin)], 
	digitalRead (pin) == HIGH ? "High" : "Low ") ;
  }

  printf ("+----------+------+------+--------+------+-------+\n") ;
}
