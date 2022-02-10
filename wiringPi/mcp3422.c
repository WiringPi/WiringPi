/*
 * mcp3422.c:
 *	Extend wiringPi with the MCP3422/3/4 I2C ADC chip
 *	This code assumes single-ended mode only.
 *	Tested on actual hardware: 20th Feb 2016.
 *	Copyright (c) 2013-2016 Gordon Henderson
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


#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "mcp3422.h"


/*
 * waitForConversion:
 *	Common code to wait for the ADC to finish conversion
 *********************************************************************************
 */

void waitForConversion (int fd, unsigned char *buffer, int n)
{
  for (;;)
  {
    read (fd, buffer, n) ;
    if ((buffer [n-1] & 0x80) == 0)
      break ;
    delay (1) ;
  }
}

/*
 * myAnalogRead:
 *	Read a channel from the device
 *********************************************************************************
 */

int myAnalogRead (struct wiringPiNodeStruct *node, int chan)
{
  unsigned char config ;
  unsigned char buffer [4] ;
  int value = 0 ;
  int realChan = (chan & 3) - node->pinBase ;

// One-shot mode, trigger plus the other configs.

  config = 0x80 | (realChan << 5) | (node->data0 << 2) | (node->data1) ;
  
  wiringPiI2CWrite (node->fd, config) ;

  switch (node->data0)	// Sample rate
  {
    case MCP3422_SR_3_75:			// 18 bits
      waitForConversion (node->fd, &buffer [0], 4) ;
      value = ((buffer [0] & 3) << 16) | (buffer [1] << 8) | buffer [2] ;
      break ;

    case MCP3422_SR_15:				// 16 bits
      waitForConversion (node->fd, buffer, 3) ;
      value = (buffer [0] << 8) | buffer [1] ;
      break ;

    case MCP3422_SR_60:				// 14 bits
      waitForConversion (node->fd, buffer, 3) ;
      value = ((buffer [0] & 0x3F) << 8) | buffer [1] ;
      break ;

    case MCP3422_SR_240:			// 12 bits - default
      waitForConversion (node->fd, buffer, 3) ;
      value = ((buffer [0] & 0x0F) << 8) | buffer [1] ;
      break ;
  }

  return value ;
}


/*
 * mcp3422Setup:
 *	Create a new wiringPi device node for the mcp3422
 *********************************************************************************
 */

int mcp3422Setup (int pinBase, int i2cAddress, int sampleRate, int gain)
{
  int fd ;
  struct wiringPiNodeStruct *node ;

  if ((fd = wiringPiI2CSetup (i2cAddress)) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, 4) ;

  node->fd         = fd ;
  node->data0      = sampleRate ;
  node->data1      = gain ;
  node->analogRead = myAnalogRead ;

  return TRUE ;
}
