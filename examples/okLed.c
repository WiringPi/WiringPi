/*
 * okLed.c:
 *      Make the OK LED on the Pi Pulsate...
 *
 * Originally posted to the Raspberry Pi forums:
 *  http://www.raspberrypi.org/phpBB3/viewtopic.php?p=162581#p162581
 *
 * Compile this and store it somewhere, then kick it off at boot time
 *    e.g. by putting it in /etc/rc.local and running it in the
 *    background &
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
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <wiringPi.h>
#include <softPwm.h>

// The OK/Act LED is connected to BCM_GPIO pin 16

#define OK_LED  16

int main ()
{
  int fd, i ;

  wiringPiSetupGpio () ;

// Change the trigger on the OK/Act LED to "none"

  if ((fd = open ("/sys/class/leds/led0/trigger", O_RDWR)) < 0)
  {
    fprintf (stderr, "Unable to change LED trigger: %s\n", strerror (errno)) ;
    return 1 ;
  }
  write (fd, "none\n", 5) ;
  close (fd) ;

  softPwmCreate (OK_LED, 0, 100) ;

  for (;;)
  {
    for (i = 0 ; i <= 100 ; ++i)
    {
      softPwmWrite (OK_LED, i) ;
      delay (10) ;
    }
    delay (50) ;

    for (i = 100 ; i >= 0 ; --i)
    {
      softPwmWrite (OK_LED, i) ;
      delay (10) ;
    }
    delay (10) ;
  }

  return 0 ;
}
