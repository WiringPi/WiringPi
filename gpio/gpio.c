/*
 * gpio.c:
 *	Set-UID command-line interface to the Raspberry Pi's GPIO
 *	Copyright (c) 2012 Gordon Henderson
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

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>


#define	VERSION	"1.0"

static int wpMode ;

char *usage = "Usage: gpio -v\n"
              "       gpio [-g] <read/write/pwm/mode> ...\n"
              "       gpio [-p] <read/write/mode> ...\n"
	      "       gpio <export/edge/unexport/unexportall/exports> ..." ;


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
  char buf [16] ;

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

    if ((l = read (fd, buf, 16)) == 0)
      sprintf (buf, "%s", "?") ;
 
    buf [l] = 0 ;
    if ((buf [strlen (buf) - 1]) == '\n')
      buf [strlen (buf) - 1] = 0 ;

    printf ("%-3s", buf) ;

    close (fd) ;

// Try to Read the value

    sprintf (fName, "/sys/class/gpio/gpio%d/value", i) ;
    if ((fd = open (fName, O_RDONLY)) == -1)
    {
      printf ("No Value file (huh?)\n") ;
      continue ;
    }

    if ((l = read (fd, buf, 16)) == 0)
      sprintf (buf, "%s", "?") ;

    buf [l] = 0 ;
    if ((buf [strlen (buf) - 1]) == '\n')
      buf [strlen (buf) - 1] = 0 ;

    printf ("  %s", buf) ;

// Read any edge trigger file

    sprintf (fName, "/sys/class/gpio/gpio%d/edge", i) ;
    if ((fd = open (fName, O_RDONLY)) == -1)
    {
      printf ("\n") ;
      continue ;
    }

    if ((l = read (fd, buf, 16)) == 0)
      sprintf (buf, "%s", "?") ;

    buf [l] = 0 ;
    if ((buf [strlen (buf) - 1]) == '\n')
      buf [strlen (buf) - 1] = 0 ;

    printf ("  %-8s\n", buf) ;

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

  /**/ if ((strcasecmp (mode, "in")  == 0) || (strcasecmp (mode, "input")  == 0))
    fprintf (fd, "in\n") ;
  else if ((strcasecmp (mode, "out") == 0) || (strcasecmp (mode, "output") == 0))
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

// Also change ownership of the edge file - if it exists

  sprintf (fName, "/sys/class/gpio/gpio%d/edge", pin) ;
  if (chown (fName, uid, gid) != 0)
  {
    if (errno != ENOENT)	// Silently ignore File not found - older kernel
    {
      fprintf (stderr, "%s: Unable to change ownership of the value file: %s\n", argv [1], strerror (errno)) ;
      exit (1) ;
    }
  }

}


/*
 * doEdge:
 *	gpio edge pin mode
 *	Easy access to changing the edge trigger on a GPIO pin
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

void doEdge (int argc, char *argv [])
{
  FILE *fd ;
  int pin ;
  char *mode ;
  char fName [128] ;
  uid_t uid ;
  gid_t gid ;

  if (argc != 4)
  {
    fprintf (stderr, "Usage: %s edge pin mode\n", argv [0]) ;
    exit (1) ;
  }

  pin = atoi (argv [2]) ;

  mode = argv [3] ;

// Export the pin and set direction to input

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

  fprintf (fd, "in\n") ;
  fclose (fd) ;

  sprintf (fName, "/sys/class/gpio/gpio%d/edge", pin) ;
  if ((fd = fopen (fName, "w")) == NULL)
  {
    fprintf (stderr, "%s: Unable to open GPIO edge interface for pin %d: %s\n", argv [0], pin, strerror (errno)) ;
    exit (1) ;
  }

  /**/ if (strcasecmp (mode, "none")    == 0)
    fprintf (fd, "none\n") ;
  else if (strcasecmp (mode, "rising")  == 0)
    fprintf (fd, "rising\n") ;
  else if (strcasecmp (mode, "falling") == 0)
    fprintf (fd, "falling\n") ;
  else if (strcasecmp (mode, "both")    == 0)
    fprintf (fd, "both\n") ;
  else
  {
    fprintf (stderr, "%s: Invalid mode: %s. Should be none, rising, falling or both\n", argv [1], mode) ;
    exit (1) ;
  }

// Change ownership so the current user can actually use it!

  uid = getuid () ;
  gid = getgid () ;

  sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
  if (chown (fName, uid, gid) != 0)
  {
    fprintf (stderr, "%s: Unable to change ownership of the value file: %s\n", argv [1], strerror (errno)) ;
    exit (1) ;
  }

// Also change ownership of the edge file

  sprintf (fName, "/sys/class/gpio/gpio%d/edge", pin) ;
  if (chown (fName, uid, gid) != 0)
  {
    fprintf (stderr, "%s: Unable to change ownership of the value file: %s\n", argv [1], strerror (errno)) ;
    exit (1) ;
  }

  fclose (fd) ;
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
 * doUnexportAll:
 *	gpio unexportall
 *	Un-Export all the GPIO pins.
 *	This uses the /sys/class/gpio device interface.
 *********************************************************************************
 */

void doUnexportall (int argc, char *argv [])
{
  FILE *fd ;
  int pin ;

  for (pin = 0 ; pin < 63 ; ++pin)
  {
    if ((fd = fopen ("/sys/class/gpio/unexport", "w")) == NULL)
    {
      fprintf (stderr, "%s: Unable to open GPIO export interface\n", argv [0]) ;
      exit (1) ;
    }
    fprintf (fd, "%d\n", pin) ;
    fclose (fd) ;
  }
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

  if (strcasecmp (argv [1], "-v") == 0)
  {
    printf ("gpio version: %s\n", VERSION) ;
    printf ("Copyright (c) 2012 Gordon Henderson\n") ;
    printf ("This is free software with ABSOLUTELY NO WARRANTY.\n") ;
    return 0 ;
  }

// Initial test for /sys/class/gpio operations:

  /**/ if (strcasecmp (argv [1], "exports" ) == 0)
    { doExports () ;			return 0 ; }
  else if (strcasecmp (argv [1], "export"  ) == 0)
    { doExport (argc, argv) ;		return 0 ; }
  else if (strcasecmp (argv [1], "edge"    ) == 0)
    { doEdge (argc, argv) ;		return 0 ; }
  else if (strcasecmp (argv [1], "unexportall") == 0)
    { doUnexportall (argc, argv) ;	return 0 ; }
  else if (strcasecmp (argv [1], "unexport") == 0)
    { doUnexport (argc, argv) ;		return 0 ; }

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

// Check for -p argument for PiFace

  else if (strcasecmp (argv [1], "-p") == 0)
  {
    if (wiringPiSetupPiFaceForGpioProg () == -1)
    {
      fprintf (stderr, "%s: Unable to initialise PiFace.\n", argv [0]) ;
      exit (1) ;
    }

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
    wpMode = WPI_MODE_PIFACE ;
  }

// Default to wiringPi mode

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
    fprintf (stderr, "%s: Unknown command: %s. (read/write/pwm/mode expected)\n", argv [0], argv [1]) ;
    exit (1) ;
  }
  return 0 ;
}
