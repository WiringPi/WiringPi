/*
 * daemonise.c:
 *	Fairly generic "Turn the current process into a daemon" code.
 *
 *	Copyright (c) 2016-2017 Gordon Henderson.
 *********************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/stat.h>

#include "daemonise.h"

void daemonise (const char *pidFile)
{
  pid_t pid ;
  int i ;
  FILE *fd ;

  syslog (LOG_DAEMON | LOG_INFO, "Becoming daemon") ;

// Fork from the parent

  if ((pid = fork ()) < 0)
  {
    syslog (LOG_DAEMON | LOG_ALERT, "Fork no. 1 failed: %m") ;
    exit (EXIT_FAILURE) ;
  }

  if (pid > 0)			// Parent - terminate
    exit (EXIT_SUCCESS) ;

// Now running on the child - become session leader

  if (setsid() < 0)
  {
    syslog (LOG_DAEMON | LOG_ALERT, "setsid failed: %m") ;
    exit (EXIT_FAILURE) ;
  }

// Ignore a few signals

  signal (SIGCHLD, SIG_IGN) ;
  signal (SIGHUP,  SIG_IGN) ;

// Fork again

  if ((pid = fork ()) < 0)
  {
    syslog (LOG_DAEMON | LOG_ALERT, "Fork no. 2 failed: %m") ;
    exit (EXIT_FAILURE) ;
  }

  if (pid > 0)			// parent - terminate
    exit (EXIT_SUCCESS) ;

// Tidying up - reset umask, change to / and close all files

  umask (0) ;
  chdir ("/") ;

  for (i = 0 ; i < sysconf (_SC_OPEN_MAX) ; ++i)
    close (i) ;

// Write PID into /var/run

  if (pidFile != NULL)
  {
    if ((fd = fopen (pidFile, "w")) == NULL)
    {
      syslog (LOG_DAEMON | LOG_ALERT, "Unable to write PID file: %m") ;
      exit (EXIT_FAILURE) ;
    }

    fprintf (fd, "%d\n", getpid ()) ;
    fclose (fd) ;
  }
}
