/*
 * network.c:
 *	Part of wiringPiD
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>

#include <fcntl.h>
#include <crypt.h>

#include "network.h"

#define	TRUE	(1==1)
#define	FALSE	(!TRUE)

// Local data

#define	SALT_LEN	16

static char salt [SALT_LEN + 1] ;
static char *returnedHash = NULL ;
static int serverFd = -1 ;

// Union for the server Socket Address

static union
{
  struct sockaddr_in  sin ;
  struct sockaddr_in6 sin6 ;
} serverSockAddr ; 

// and client address

static union
{
  struct sockaddr_in  sin ;
  struct sockaddr_in6 sin6 ;
} clientSockAddr ;


/*
 * getClientIP:
 *	Returns a pointer to a static string containing the clients IP address
 *********************************************************************************
 */

char *getClientIP (void)
{
  char buf [INET6_ADDRSTRLEN] ;
  static char ipAddress [1024] ;

  if (clientSockAddr.sin.sin_family == AF_INET)	// IPv4
  {
    if (snprintf (ipAddress, 1024, "IPv4: %s", 
	inet_ntop (clientSockAddr.sin.sin_family, (void *)&clientSockAddr.sin.sin_addr, buf, sizeof (buf))) == 1024)
      strcpy (ipAddress, "Too long") ;
  }
  else						// IPv6
  {
    if (clientSockAddr.sin.sin_family == AF_INET6 && IN6_IS_ADDR_V4MAPPED (&clientSockAddr.sin6.sin6_addr))
    {
      if (snprintf (ipAddress, 1024, "IPv4in6: %s", 
	inet_ntop (clientSockAddr.sin.sin_family, (char *)&clientSockAddr.sin6.sin6_addr, buf, sizeof(buf))) == 1024)
      strcpy (ipAddress, "Too long") ;
    }
    else
    {
      if (snprintf (ipAddress, 1024, "IPv6: %s", 
	inet_ntop (clientSockAddr.sin.sin_family, (char *)&clientSockAddr.sin6.sin6_addr, buf, sizeof(buf))) == 1024)
      strcpy (ipAddress, "Too long") ;
    }
  }

  return ipAddress ;
}



/*
 * clientPstr: clientPrintf:
 *	Print over a network socket
 *********************************************************************************
 */

static int clientPstr (int fd, char *s)
{
  int len = strlen (s) ;
  return (write (fd, s, len) == len) ? 0 : -1 ;
}

static int clientPrintf (const int fd, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  va_start (argp, message) ;
    vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  return clientPstr (fd, buffer) ;
}


/*
 * sendGreeting:
 *	Send some text to the client device
 *********************************************************************************
 */

int sendGreeting (int clientFd)
{
  if (clientPrintf (clientFd, "200 Welcome to wiringPiD - http://wiringpi.com/\n") < 0)
    return -1 ;

  return clientPrintf (clientFd, "200 Connecting from: %s\n", getClientIP ()) ;
}


/*
 * getSalt:
 *	Create a random 'salt' value for the password encryption process
 *********************************************************************************
 */

static int getSalt (char drySalt [])
{
  static const char *seaDog =	"abcdefghijklmnopqrstuvwxyz"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"0123456789/." ;

  unsigned char wetSalt [SALT_LEN] ;
  int i, fd ;

  if ((fd = open ("/dev/urandom", O_RDONLY)) < 0)
    return fd ;

  if (read (fd, wetSalt, SALT_LEN) != SALT_LEN)
    return -1 ;

  close (fd) ;

  for (i = 0 ; i < SALT_LEN ; ++i)
    drySalt [i] = seaDog [wetSalt [i] & 63] ;
    
  drySalt [SALT_LEN] = 0 ;

  return 0 ;
}


/*
 * sendChallenge:
 *	Create and send our salt (aka nonce) to the remote device
 *********************************************************************************
 */

int sendChallenge (int clientFd)
{
  if (getSalt (salt) < 0)
    return -1 ;

  return clientPrintf (clientFd, "Challenge %s\n", salt) ;
}


/*
 * getResponse:
 *	Read the encrypted password from the remote device.
 *********************************************************************************
 */


int getResponse (int clientFd)
{
  char reply [1024] ;
  int len ;

// Being sort of lazy about this. I'm expecting an SHA-512 hash back and these
//	are exactly 86 characters long, so no reason not to, I guess...

  len = 86 ;

  if (setsockopt (clientFd, SOL_SOCKET, SO_RCVLOWAT, (void *)&len, sizeof (len)) < 0)
    return -1 ;

  len = recv (clientFd, reply, 86, 0) ;
  if (len != 86)
    return -1 ;

  reply [len] = 0 ;

  if ((returnedHash = malloc (len + 1)) == NULL)
    return -1 ;

  strcpy (returnedHash, reply) ;

  return 0 ;
}


/*
 * passwordMatch:
 *	See if there's a match. If not, we simply dump them.
 *********************************************************************************
 */

int passwordMatch (const char *password)
{
  char *encrypted ;
  char salted [1024] ;

  sprintf (salted, "$6$%s$", salt) ;

  encrypted = crypt (password, salted) ;

// 20: $6$ then 16 characters of salt, then $
// 86 is the length of an SHA-512 hash

  return strncmp (encrypted + 20, returnedHash, 86) == 0 ;
}


/* 
 * setupServer:
 *	Do what's needed to create a local server socket instance that can listen
 *	on both IPv4 and IPv6 interfaces.
 *********************************************************************************
 */

int setupServer (int serverPort)
{
  socklen_t clientSockAddrSize = sizeof (clientSockAddr) ;

  int on = 1 ;
  int family ;
  socklen_t serverSockAddrSize ;
  int clientFd ;

// Try to create an IPv6 socket

  serverFd = socket (PF_INET6, SOCK_STREAM, 0) ;

// If it didn't work, then fall-back to IPv4.

  if (serverFd < 0)
  {
    if ((serverFd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
      return -1 ;

    family             = AF_INET ;
    serverSockAddrSize = sizeof (struct sockaddr_in) ;
  }
  else		// We got an IPv6 socket
  {
    family             = AF_INET6 ;
    serverSockAddrSize = sizeof (struct sockaddr_in6) ;
  }

  if (setsockopt (serverFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) < 0)
    return -1 ;

// Setup the servers socket address - cope with IPv4 and v6.

  memset (&serverSockAddr, 0, sizeof (serverSockAddr)) ;
  switch (family)
  {
    case AF_INET:
      serverSockAddr.sin.sin_family      = AF_INET ;
      serverSockAddr.sin.sin_addr.s_addr = htonl (INADDR_ANY) ;
      serverSockAddr.sin.sin_port        = htons (serverPort) ;
      break;

    case AF_INET6:
      serverSockAddr.sin6.sin6_family = AF_INET6 ;
      serverSockAddr.sin6.sin6_addr   = in6addr_any ;
      serverSockAddr.sin6.sin6_port   = htons (serverPort) ;
  }

// Bind, listen and accept

  if (bind (serverFd, (struct sockaddr *)&serverSockAddr, serverSockAddrSize) < 0)
    return -1 ;

  if (listen (serverFd, 4) < 0)	// Really only going to talk to one client at a time...
    return -1 ;

  if ((clientFd = accept (serverFd, (struct sockaddr *)&clientSockAddr, &clientSockAddrSize)) < 0)
    return -1 ;

  return clientFd ;
}


/*
 * closeServer:
 *********************************************************************************
 */

void closeServer (int clientFd)
{
  if (serverFd != -1) close (serverFd) ;
  if (clientFd != -1) close (clientFd) ;
  serverFd = clientFd = -1 ;
}
