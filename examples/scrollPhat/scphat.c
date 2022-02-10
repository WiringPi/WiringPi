/*
 * scphat.c:
 *	Little program to allow use of the Pimoroni Scroll pHAT
 *	from the command-line.
 *
 * Copyright (c) 2015-2016 Gordon Henderson.
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>
#include <scrollPhat.h>

static char *progName ;


/*
 * checkArgs:
 *	Count the arguments for each little function
 *********************************************************************************
 */

static void checkArgs (char *command, int num, int arg, int argc)
{
  if ((arg + num) < argc)
    return ;

  fprintf (stderr, "%s: Not enough data for %s command.\n", progName, command) ;
  exit (EXIT_FAILURE) ;
}


/*
 * doClear:
 *	Clear the display
 *********************************************************************************
 */

static int doClear (void)
{
  scrollPhatClear () ;
  return 1 ;
}


/*
 * doBright
 *********************************************************************************
 */

static int doBright (int arg, int argc, char *argv [])
{
  checkArgs ("bright", 1, arg, argc) ;
  scrollPhatIntensity (atoi (argv [arg+1])) ;
  return 2 ;
}



/*
 * doPlot
 *********************************************************************************
 */

static int doPlot (int arg, int argc, char *argv [])
{
  checkArgs ("plot", 2, arg, argc) ;
  scrollPhatPoint (atoi (argv [arg+1]), atoi (argv [arg+2]), 1) ;
  scrollPhatUpdate () ;
  return 3 ;
}


/*
 * doLine
 *********************************************************************************
 */

static int doLine (int arg, int argc, char *argv [])
{
  checkArgs ("line", 4, arg, argc) ;
  scrollPhatLine (atoi (argv [arg+1]), atoi (argv [arg+2]),
  			atoi (argv [arg+3]), atoi (argv [arg+4]), 1) ;
  scrollPhatUpdate () ;
  return 5 ;
}


/*
 * doLineTo
 *********************************************************************************
 */

static int doLineTo (int arg, int argc, char *argv [])
{
  checkArgs ("lineto", 2, arg, argc) ;
  scrollPhatLineTo (atoi (argv [arg+1]), atoi (argv [arg+2]), 1) ;
  scrollPhatUpdate () ;
  return 3 ;
}


/*
 * doWait
 *********************************************************************************
 */

static int doWait (int arg, int argc, char *argv [])
{
  checkArgs ("wait", 1, arg, argc) ;
  delay (atoi (argv [arg+1]) * 100) ;
  scrollPhatUpdate () ;
  return 2 ;
}


/*
 * doSpeed
 *********************************************************************************
 */

static int doSpeed (int arg, int argc, char *argv [])
{
  checkArgs ("speed", 1, arg, argc) ;
  scrollPhatPrintSpeed (atoi (argv [arg+1])) ;
  return 2 ;
}


/*
 * doScroll
 *********************************************************************************
 */

static int doScroll (int arg, int argc, char *argv [])
{
  checkArgs ("scroll", 1, arg, argc) ;
  scrollPhatPuts (argv [arg+1]) ;
  return 2 ;
}


static void failUsage (void)
{
  fprintf (stderr, "Usage: %s command [paremters] ...\n", progName) ;
  fprintf (stderr, "  commands:\n") ;
  fprintf (stderr, "    clear/cls        - Clear the display\n") ;
  fprintf (stderr, "    bright N         - Set display brightness; 1-100\n") ;
  fprintf (stderr, "    plot X Y         - Set a single pixel at location X Y; 0-10, 0-4\n") ;
  fprintf (stderr, "    line X1 Y1 X2 Y2 - Draw a line from the 2 points\n") ;
  fprintf (stderr, "    lineto X2 Y2     - Draw a line from the last point to the new one\n") ;
  fprintf (stderr, "    wait/delay N     - Wait for N 10ths seconds\n") ;
  fprintf (stderr, "    speed N          - Set scrolling speed (cps)\n") ;
  fprintf (stderr, "    scroll S         - Scroll the given string\n") ;
  fprintf (stderr, "\n") ;
  fprintf (stderr, "  Example: %s plot 0 0 wait 50 scroll \"  Hello  \"\n", progName) ;
  exit (EXIT_FAILURE) ;
}


/*
 * the works
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int arg = 1 ;
  char *command ;

  progName = argv [0] ;

  wiringPiSetupSys () ;

  if (scrollPhatSetup () != 0)
  {
    fprintf (stderr, "%s: Unable to initialise the scrollPhat: %s\n", progName, strerror (errno)) ;
    exit (EXIT_FAILURE) ;
  }

  progName = argv [0] ;

  if (argc < 2)
  {
    fprintf (stderr, "%s: Nothing to do...\n", argv [0]) ;
    failUsage () ;
  }

  while (arg != argc)
  {
    command = argv [arg] ;
    /**/ if (strcasecmp (command, "clear")  == 0) arg += doClear  () ;
    else if (strcasecmp (command, "cls")    == 0) arg += doClear  () ;
    else if (strcasecmp (command, "bright") == 0) arg += doBright (arg, argc, argv) ;
    else if (strcasecmp (command, "plot")   == 0) arg += doPlot   (arg, argc, argv) ;
    else if (strcasecmp (command, "line")   == 0) arg += doLine   (arg, argc, argv) ;
    else if (strcasecmp (command, "lineto") == 0) arg += doLineTo (arg, argc, argv) ;
    else if (strcasecmp (command, "wait")   == 0) arg += doWait   (arg, argc, argv) ;
    else if (strcasecmp (command, "delay")  == 0) arg += doWait   (arg, argc, argv) ;
    else if (strcasecmp (command, "speed")  == 0) arg += doSpeed  (arg, argc, argv) ;
    else if (strcasecmp (command, "scroll") == 0) arg += doScroll (arg, argc, argv) ;
    else
    {
      fprintf (stderr, "%s: Unknown command: %s.\n", argv [0], argv [arg]) ;
      failUsage () ;
    }
  }

  return 0 ;
}
