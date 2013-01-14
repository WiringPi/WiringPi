/*
 * okLed:
 *      Make the OK LED on the Pi Pulsate...
 *    Copyright (c) 2012 gordon Henderson, but please Share and Enjoy!
 *
 * Originally posted to the Raspberry Pi forums:
 *  http://www.raspberrypi.org/phpBB3/viewtopic.php?p=162581#p162581
 *
 * Compile this and store it somewhere, then kick it off at boot time
 *    e.g. by putting it in /etc/rc.local and running it in the
 *    background &
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#include <wiringPi.h>
#include <softPwm.h>

#define OK_LED  16

int main ()
{
  int fd, i ;

  if ((fd = open ("/sys/class/leds/led0/trigger", O_RDWR)) < 0)
  {
    fprintf (stderr, "Unable to change LED trigger: %s\n", strerror (errno)) ;
    return 1 ;
  }

  write (fd, "none\n", 5) ;
  close (fd) ;

  if (wiringPiSetupGpio () < 0)
  {
    fprintf (stderr, "Unable to setup GPIO: %s\n", strerror (errno)) ;
    return 1 ;
  }

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
