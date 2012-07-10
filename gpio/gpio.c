/*
 * gpio.c:
 *	Set-UID command-line interface to the Raspberry Pi's GPIO
 *	Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>


static int wpMode ;


char *usage = "Usage: gpio [-g] <read/write/pwm/mode/export/unexport/exports> ..." ;


/*
 * doExports:
 *	List all GPIO exports
 *********************************************************************************
 */

void doExports (void)
{
  int fd ;
  int i, l, first ;
  char fName [128] ;
  char dir, val ;

// Rather crude, but who knows what others are up to...

  for (first = 0, i = 0 ; i < 64 ; ++i)
  {

// Try to read the direction

    sprintf (fName, "/sys/class/gpio/gpio%d/direction", i) ;
    if ((fd = open (fName, O_RDONLY)) == -1)
      continue ;

    if (first == 0)
    {
      ++first ;
      printf ("GPIO Pins exported:\n") ;
    }

    printf ("%4d: ", i) ;

    if ((l = read (fd, &dir, 1)) == 0)
    {
      printf ("Empty direction file (why?)\n") ;
      close (fd) ;
      continue ;
    }

    /**/ if (dir == 'o')
      printf ("Output ") ;
    else if (dir == 'i')
      printf ("Input  ") ;
    else
      printf ("Wrong  ") ;

    close (fd) ;

// Try to Read the value

    sprintf (fName, "/sys/class/gpio/gpio%d/value", i) ;
    if ((fd = open (fName, O_RDONLY)) == -1)
    {
      printf ("No Value file (huh?)\n") ;
      continue ;
    }

    if ((l = read (fd, &val, 1)) == 0)
    {
      printf ("Empty Value file (why?)\n") ;
      close (fd) ;
      continue ;
    }

    /**/ if (val == '0' )
      printf ("(0)\n") ;
    else if (val == '1')
      printf ("(1)\n") ;
    else
      printf ("(?)\n") ;

    close (fd) ;
  }
}


/*
 * doExport:
 *	gpio export pin mode
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

void doExport (int argc, char *argv [])
{
  FILE *fd ;
  int pin ;
  char *mode ;
  char fName [128] ;
  uid_t uid ;
  gid_t gid ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s export pin mode\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  mode = argv [3] ;

  if ((fd = fopen ("/sys/class/gpio/export", "w")) == NULL)
  {
    fprintf (stderr, "%s: Unable to open GPIO export interface: %s\n", argv [0], strerror (errno)) ;
    exit (1) ;
  }

  fprintf (fd, "%d\n", pin) ;
  fclose (fd) ;

  sprintf (fName, "/sys/class/gpio/gpio%d/direction", pin) ;
  if ((fd = fopen (fName, "w")) == NULL)
  {
    fprintf (stderr, "%s: Unable to open GPIO direction interface for pin %d: %s\n", argv [0], pin, strerror (errno)) ;
    exit (1) ;
  }

  /**/ if (strcasecmp (mode, "in")   == 0)
    fprintf (fd, "in\n") ;
  else if (strcasecmp (mode, "out")  == 0)
    fprintf (fd, "out\n") ;
  else
  {
    fprintf (stderr, "%s: Invalid mode: %s. Should be in or out\n", argv [1], mode) ;
    exit (1) ;
  }

  fclose (fd) ;

// Change ownership so the current user can actually use it!

  uid = getuid () ;
  gid = getgid () ;

  sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
  if (chown (fName, uid, gid) != 0)
  {
    fprintf (stderr, "%s: Unable to change ownership of the value file: %s\n", argv [1], strerror (errno)) ;
    exit (1) ;
  }
  
}


/*
 * doUnexport:
 *	gpio unexport pin
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

void doUnexport (int argc, char *argv [])
{
  FILE *fd ;
  int pin ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s unexport pin\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if ((fd = fopen ("/sys/class/gpio/unexport", "w")) == NULL)
  {
    fprintf (stderr, "%s: Unable to open GPIO export interface\n", argv [0]) ;
    exit (1) ;
  }

  fprintf (fd, "%d\n", pin) ;
  fclose (fd) ;
}

/*
 * doMode:
 *	gpio mode pin mode ...
 *********************************************************************************
 */

void doMode (int argc, char *argv [])
{
  int pin ;
  char *mode ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s mode pin mode\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if ((wpMode == WPI_MODE_PINS) && ((pin < 0) || (pin >= NUM_PINS)))
    return ;

  mode = argv [3] ;

  /**/ if (strcasecmp (mode, "in")   == 0)
    pinMode (pin, INPUT) ;
  else if (strcasecmp (mode, "out")  == 0)
    pinMode (pin, OUTPUT) ;
  else if (strcasecmp (mode, "pwm")  == 0)
    pinMode (pin, PWM_OUTPUT) ;
  else if (strcasecmp (mode, "up")   == 0)
    pullUpDnControl (pin, PUD_UP) ;
  else if (strcasecmp (mode, "down") == 0)
    pullUpDnControl (pin, PUD_DOWN) ;
  else if (strcasecmp (mode, "tri") == 0)
    pullUpDnControl (pin, PUD_OFF) ;
  else
  {
    fprintf (stderr, "%s: Invalid mode: %s. Should be in/out/pwm/up/down/tri\n", argv [1], mode) ;
    exit (1) ;
  }
}

/*
 * doWrite:
 *	gpio write pin value
 *********************************************************************************
 */

void doWrite (int argc, char *argv [])
{
  int pin, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s write pin value\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if ((wpMode == WPI_MODE_PINS) && ((pin < 0) || (pin >= NUM_PINS)))
    return ;

  val = atoi (argv [3]) ;

  /**/ if (val == 0)
    digitalWrite (pin, LOW) ;
  else
    digitalWrite (pin, HIGH) ;
}


/*
 * doRead:
 *	Read a pin and return the value
 *********************************************************************************
 */

void doRead (int argc, char *argv []) 
{
  int pin, val ;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s read pin\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if ((wpMode == WPI_MODE_PINS) && ((pin < 0) || (pin >= NUM_PINS)))
  {
    printf ("0\n") ;
    return ;
  }

  val = digitalRead (pin) ;

  printf ("%s\n", val == 0 ? "0" : "1") ;
}


/*
 * doPwm:
 *	Output a PWM value on a pin
 *********************************************************************************
 */

void doPwm (int argc, char *argv [])
{
  int pin, val ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s pwm <pin> <value>\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  if ((wpMode == WPI_MODE_PINS) && ((pin < 0) || (pin >= NUM_PINS)))
    return ;

  val = atoi (argv [3]) ;

  pwmWrite (pin, val) ;
}


/*
 * main:
 *	Start here
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int i ;

  if (argc == 1)
  {
    fprintf (stderr, "%s: %s\n", argv [0], usage) ;
    return 1 ;
  }

  if (geteuid () != 0)
  {
    fprintf (stderr, "%s: Must be root to run\n", argv [0]) ;
    return 1 ;
  }

// Initial test for /sys/class/gpio operations:

  /**/ if (strcasecmp (argv [1], "exports" ) == 0)
    { doExports () ;		return 0 ; }
  else if (strcasecmp (argv [1], "export"  ) == 0)
    { doExport (argc, argv) ;	return 0 ; }
  else if (strcasecmp (argv [1], "unexport") == 0)
    { doUnexport (argc, argv) ;	return 0 ; }

// Check for -g argument

  if (strcasecmp (argv [1], "-g") == 0)
  {
    if (wiringPiSetupGpio () == -1)
    {
      fprintf (stderr, "%s: Unable to initialise GPIO in GPIO mode.\n", argv [0]) ;
      exit (1) ;
    }

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_GPIO ;
  }
  else
  {
    if (wiringPiSetup () == -1)
    {
      fprintf (stderr, "%s: Unable to initialise GPIO in wiringPi mode\n", argv [0]) ;
      exit (1) ;
    }
    wpMode = WPI_MODE_PINS ;
  }

  /**/ if (strcasecmp (argv [1], "write"   ) == 0)
    doWrite  (argc, argv) ;
  else if (strcasecmp (argv [1], "read"    ) == 0)
    doRead   (argc, argv) ;
  else if (strcasecmp (argv [1], "mode"    ) == 0)
    doMode   (argc, argv) ;
  else if (strcasecmp (argv [1], "pwm"     ) == 0)
    doPwm    (argc, argv) ;
  else
  {
    fprintf (stderr, "%s: Unknown command: %s. (read/write/pwm/mode/export/unexport expected)\n", argv [0], argv [1]) ;
    exit (1) ;
  }
  return 0 ;
}
