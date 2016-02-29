/*
 * drcSerial.c:
 *	Extend wiringPi with the DRC Serial protocol (e.g. to Arduino)
 *	Copyright (c) 2013-2016 Gordon Henderson
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

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include "wiringPi.h"
#include "wiringSerial.h"

#include "drcSerial.h"


/*
 * myPinMode:
 *	Change the pin mode on the remote DRC device
 *********************************************************************************
 */

static void myPinMode (struct wiringPiNodeStruct *node, int pin, int mode)
{
  /**/ if (mode == OUTPUT)
    serialPutchar (node->fd, 'o') ;       // Input
  else if (mode == PWM_OUTPUT)
    serialPutchar (node->fd, 'p') ;       // PWM
  else
    serialPutchar (node->fd, 'i') ;       // Default to input

  serialPutchar (node->fd, pin - node->pinBase) ;
}


/*
 * myPullUpDnControl:
 *	ATmegas only have pull-up's on of off. No pull-downs.
 *********************************************************************************
 */

static void myPullUpDnControl (struct wiringPiNodeStruct *node, int pin, int mode)
{

// Force pin into input mode

  serialPutchar (node->fd, 'i' ) ;
  serialPutchar (node->fd, pin - node->pinBase) ;

  /**/ if (mode == PUD_UP)
  {
    serialPutchar (node->fd, '1') ;
    serialPutchar (node->fd, pin - node->pinBase) ;
  }
  else if (mode == PUD_OFF)
  {
    serialPutchar (node->fd, '0') ;
    serialPutchar (node->fd, pin - node->pinBase) ;
  }
}


/*
 * myDigitalWrite:
 *********************************************************************************
 */

static void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  serialPutchar (node->fd, value == 0 ? '0' : '1') ;
  serialPutchar (node->fd, pin - node->pinBase) ;
}


/*
 * myPwmWrite:
 *********************************************************************************
 */

static void myPwmWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  serialPutchar (node->fd, 'v') ;
  serialPutchar (node->fd, pin - node->pinBase) ;
  serialPutchar (node->fd, value & 0xFF) ;
}


/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  int vHi, vLo ;

  serialPutchar (node->fd, 'a') ;
  serialPutchar (node->fd, pin - node->pinBase) ;
  vHi = serialGetchar (node->fd) ;
  vLo = serialGetchar (node->fd) ;

  return (vHi << 8) | vLo ;
}


/*
 * myDigitalRead:
 *********************************************************************************
 */

static int myDigitalRead (struct wiringPiNodeStruct *node, int pin)
{
  serialPutchar (node->fd, 'r') ; // Send read command
  serialPutchar (node->fd, pin - node->pinBase) ;
  return (serialGetchar (node->fd) == '0') ? 0 : 1 ;
}


/*
 * drcSetup:
 *	Create a new instance of an DRC GPIO interface.
 *	Could be a variable nunber of pins here - we might not know in advance
 *	if it's an ATmega with 14 pins, or something with less or more!
 *********************************************************************************
 */

int drcSetupSerial (const int pinBase, const int numPins, const char *device, const int baud)
{
  int fd ;
  int ok, tries ;
  time_t then ;
  struct wiringPiNodeStruct *node ;

  if ((fd = serialOpen (device, baud)) < 0)
    return FALSE ;

  delay (10) ;	// May need longer if it's an Uno that reboots on the open...

// Flush any pending input

  while (serialDataAvail (fd))
    (void)serialGetchar (fd) ;

  ok = FALSE ;
  for (tries = 1 ; (tries < 5) && (!ok) ; ++tries)
  {
    serialPutchar (fd, '@') ;		// Ping
    then = time (NULL) + 2 ;
    while (time (NULL) < then)
      if (serialDataAvail (fd))
      {
        if (serialGetchar (fd) == '@')
        {
          ok = TRUE ;
          break ;
        }
      }
  }

  if (!ok)
  {
    serialClose (fd) ;
    return FALSE ;
  }

  node = wiringPiNewNode (pinBase, numPins) ;

  node->fd              = fd ;
  node->pinMode         = myPinMode ;
  node->pullUpDnControl = myPullUpDnControl ;
  node->analogRead      = myAnalogRead ;
  node->digitalRead     = myDigitalRead ;
  node->digitalWrite    = myDigitalWrite ;
  node->pwmWrite        = myPwmWrite ;

  return TRUE ;
}
