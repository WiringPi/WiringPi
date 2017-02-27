/*
 * runRemote.c:
 *	Run the remote commands passed over the network link.
 *
 *	Copyright (c) 2012-2017 Gordon Henderson
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

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
//#include <stdarg.h>

#include <wiringPi.h>
#include <wpiExtensions.h>

#include "drcNetCmd.h"
#include "network.h"
#include "runRemote.h"



int noLocalPins = FALSE ;


void runRemoteCommands (int fd)
{
  register uint32_t pin ;
  int len ;
  struct drcNetComStruct cmd ;

  len = sizeof (struct drcNetComStruct) ;

  if (setsockopt (fd, SOL_SOCKET, SO_RCVLOWAT, (void *)&len, sizeof (len)) < 0)
    return ;

  for (;;)
  {
    if (recv (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))	// Probably remote hangup
      return ;

    pin = cmd.pin ;
    if (noLocalPins && ((pin & PI_GPIO_MASK) == 0))
    {
      if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	return ;
      continue ;
    }

    switch (cmd.cmd)
    {
      case DRCN_PIN_MODE:
	pinMode (pin, cmd.data) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_PULL_UP_DN:
	pullUpDnControl (pin, cmd.data) ;
	break ;

      case DRCN_PWM_WRITE:
	pwmWrite (pin, cmd.data) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_DIGITAL_WRITE:
	digitalWrite (pin, cmd.data) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_DIGITAL_WRITE8:
	//digitalWrite8 (pin, cmd.data) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_DIGITAL_READ:
	cmd.data = digitalRead (pin) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_DIGITAL_READ8:
	//cmd.data = digitalRead8 (pin) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_ANALOG_WRITE:
	analogWrite (pin, cmd.data) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;

      case DRCN_ANALOG_READ:
	cmd.data = analogRead (pin) ;
	if (send (fd, &cmd, sizeof (cmd), 0) != sizeof (cmd))
	  return ;
	break ;
    }
  }

}
