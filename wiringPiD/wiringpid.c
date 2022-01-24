/*
 * wiringPiD.c:
 *	Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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
#include <stdarg.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#include <wiringPi.h>
#include <wpiExtensions.h>

#include "drcNetCmd.h"
#include "network.h"
#include "runRemote.h"
#include "daemonise.h"


#define	PIDFILE	"/var/run/wiringPiD.pid"


// Globals

static const char *usage = "[-h] [-d] [-g | -1 | -z] [[-x extension:pin:params] ...] password" ;
static int doDaemon = FALSE ;

//

static void logMsg (const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  if (doDaemon)
    syslog (LOG_DAEMON | LOG_INFO, "%s", buffer) ;
  else
    printf ("%s\n", buffer) ;
}


/*
 * sigHandler:
 * setupSigHandler:
 *      Somehing has happened that would normally terminate the program so try
 *	to close down nicely.
 *********************************************************************************
 */

void sigHandler (int sig)
{
  logMsg ("Exiting on signal %d: %s", sig, strsignal (sig)) ;
  (void)unlink (PIDFILE) ;
  exit (EXIT_FAILURE) ;
}

void setupSigHandler (void)
{
  struct sigaction action ;

  sigemptyset (&action.sa_mask) ;
  action.sa_flags = 0 ;

// Ignore what we can

  action.sa_handler = SIG_IGN ;

  sigaction (SIGHUP,  &action, NULL) ;
  sigaction (SIGTTIN, &action, NULL) ;
  sigaction (SIGTTOU, &action, NULL) ;

// Trap what we can to exit gracefully

  action.sa_handler = sigHandler ;

  sigaction (SIGINT,  &action, NULL) ;
  sigaction (SIGQUIT, &action, NULL) ;
  sigaction (SIGILL,  &action, NULL) ;
  sigaction (SIGABRT, &action, NULL) ;
  sigaction (SIGFPE,  &action, NULL) ;
  sigaction (SIGSEGV, &action, NULL) ;
  sigaction (SIGPIPE, &action, NULL) ;
  sigaction (SIGALRM, &action, NULL) ;
  sigaction (SIGTERM, &action, NULL) ;
  sigaction (SIGUSR1, &action, NULL) ;
  sigaction (SIGUSR2, &action, NULL) ;
  sigaction (SIGCHLD, &action, NULL) ;
  sigaction (SIGTSTP, &action, NULL) ;
  sigaction (SIGBUS,  &action, NULL) ;
}


/*
 * The works...
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int clientFd ;
  char *p, *password ;
  int i ;
  int port = DEFAULT_SERVER_PORT ;
  int wpiSetup = 0 ;

  if (argc < 2)
  {
    fprintf (stderr, "Usage: %s %s\n", argv [0], usage) ;
    exit (EXIT_FAILURE) ;
  }

// Help?

  if (strcasecmp (argv [1], "-h") == 0)
  {
    printf ("Usage: %s %s\n", argv [0], usage) ;
    return 0 ;
  }

// Daemonize?
//	Must come before the other args as e.g. some extensions
//	open files which get closed on daemonise...

  if (strcasecmp (argv [1], "-d") == 0)
  {
    if (geteuid () != 0)
    {
      fprintf (stderr, "%s: Must be root to run as a daemon.\n", argv [0]) ;
      exit (EXIT_FAILURE) ;
    }

    doDaemon = TRUE ;
    daemonise (PIDFILE) ;

    for (i = 2 ; i < argc ; ++i)
      argv [i - 1] = argv [i] ;
    --argc ;
  }

// Scan all other arguments

  while (*argv [1] == '-')
  {

// Look for wiringPi setup arguments:
//	Same as the gpio command and rtb.

//	-g - bcm_gpio

    if (strcasecmp (argv [1], "-g") == 0)
    {
      if (wpiSetup == 0)
      {
	logMsg ("BCM_GPIO mode selected") ;
	wiringPiSetupGpio () ;
      }

      for (i = 2 ; i < argc ; ++i)
	argv [i - 1] = argv [i] ;
      --argc ;
      ++wpiSetup ;
      continue ;
    }

//	-1 - physical pins

    if (strcasecmp (argv [1], "-1") == 0)
    {
      if (wpiSetup == 0)
      {
	logMsg ("GPIO-PHYS mode selected") ;
	wiringPiSetupPhys () ;
      }

      for (i = 2 ; i < argc ; ++i)
	argv [i - 1] = argv [i] ;
      --argc ;
      ++wpiSetup ;
      continue ;
    }

//	-z  - no wiringPi - blocks remotes accessing local pins

    if (strcasecmp (argv [1], "-z") == 0)
    {
      if (wpiSetup == 0)
	logMsg ("No GPIO mode selected") ;

      for (i = 2 ; i < argc ; ++i)
	argv [i - 1] = argv [i] ;
      --argc ;
      noLocalPins = TRUE ;
      ++wpiSetup ;
      continue ;
    }

// -p to select the port

    if (strcasecmp (argv [1], "-p") == 0)
    {
      if (argc < 3)
      {
	logMsg ("-p missing extension port") ;
	exit (EXIT_FAILURE) ;
      }

      logMsg ("Setting port to: %s", argv [2]) ;

      port = atoi (argv [2]) ;
      if ((port < 1) || (port > 65535))
      {
	logMsg ("Invalid server port: %d", port) ;
	exit (EXIT_FAILURE) ;
      }

// Shift args down by 2

      for (i = 3 ; i < argc ; ++i)
	argv [i - 2] = argv [i] ;
      argc -= 2 ;

      continue ;
    }

// Check for -x argument to load in a new extension
//	-x extension:base:args
//	Can load many modules to extend the daemon.

    if (strcasecmp (argv [1], "-x") == 0)
    {
      if (argc < 3)
      {
	logMsg ("-x missing extension name:data:etc.") ;
	exit (EXIT_FAILURE) ;
      }

      logMsg ("Loading extension: %s", argv [2]) ;

      if (!loadWPiExtension (argv [0], argv [2], TRUE))
      {
	logMsg ("Extension load failed: %s", strerror (errno)) ;
	exit (EXIT_FAILURE) ;
      }

// Shift args down by 2

      for (i = 3 ; i < argc ; ++i)
	argv [i - 2] = argv [i] ;
      argc -= 2 ;

      continue ;
    }

    logMsg ("Invalid parameter: %s", argv [1]) ;
    exit (EXIT_FAILURE) ;
  }

// Default to wiringPi mode

  if (wpiSetup == 0)
  {
    logMsg ("WiringPi GPIO mode selected") ;
    wiringPiSetup () ;
  }

// Finally, should just be one arg left - the password...

  if (argc != 2)
  {
    logMsg ("No password supplied") ;
    exit (EXIT_FAILURE) ;
  }

  if (strlen (argv [1]) < 6)
  {
    logMsg ("Password too short - at least 6 chars, not %d", strlen (argv [1])) ;
    exit (EXIT_FAILURE) ;
  }

  if ((password = malloc (strlen (argv [1]) + 1)) == NULL)
  {
    logMsg ("Out of memory") ;
    exit (EXIT_FAILURE) ;
  }
  strcpy (password, argv [1]) ;

// Wipe out the password on the command-line in a vague attempt to try to
//	hide it from snoopers

  for (p = argv [1] ; *p ; ++p)
    *p = ' ' ;

  setupSigHandler () ;
 
// Enter our big loop

  for (;;)
  {

    if (!doDaemon)
      printf ("-=-\nWaiting for a new connection...\n") ;

    if ((clientFd = setupServer (port)) < 0)
    {
      logMsg ("Unable to setup server: %s", strerror (errno)) ;
      exit (EXIT_FAILURE) ;
    }

    logMsg ("New connection from: %s.", getClientIP ()) ;

    if (!doDaemon)
      printf ("Sending Greeting.\n") ;

    if (sendGreeting (clientFd) < 0)
    {
      logMsg ("Unable to send greeting message: %s", strerror (errno)) ;
      closeServer (clientFd) ;
      continue ;
    }

    if (!doDaemon)
      printf ("Sending Challenge.\n") ;

    if (sendChallenge (clientFd) < 0)
    {
      logMsg ("Unable to send challenge message: %s", strerror (errno)) ;
      closeServer (clientFd) ;
      continue ;
    }

    if (!doDaemon)
      printf ("Waiting for response.\n") ;

    if (getResponse (clientFd) < 0)
    {
      logMsg ("Connection closed waiting for response: %s", strerror (errno)) ;
      closeServer (clientFd) ;
      continue ;
    }

    if (!passwordMatch (password))
    {
      logMsg ("Password failure") ;
      closeServer (clientFd) ;
      continue ;
    }

    logMsg ("Password OK - Starting") ;

    runRemoteCommands (clientFd) ;
    closeServer       (clientFd) ;
  }

  return 0 ;
}
