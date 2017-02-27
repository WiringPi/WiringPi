/*
 * drcNet.h:
 *	Extend wiringPi with the DRC Network protocol (e.g. to another Pi)
 *	Copyright (c) 2016-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
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
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <crypt.h>


#include "wiringPi.h"
#include "drcNet.h"
#include "../wiringPiD/drcNetCmd.h"


/*
 * remoteReadline:
 *	Read in a line of data from the remote server, ending with a newline
 *	character which is not stored. Returns the length or < 0 on
 *	any sort of failure.
 *********************************************************************************
 */

static int remoteReadline (int fd, char *buf, int max)
{
  int  len = 0 ;
  char c ;

  for (;;)
  {
    if (read (fd, &c, 1) < 1)
      return -1 ;

    if (c == '\n')
      return len ;

    *buf++ = c ;
    if (++len == max)
      return len ;
  }
}


/*
 * getChallenge:
 *	Read in lines from the remote site until we get one identified
 *	as the challenge. This line contains the password salt.
 *********************************************************************************
 */

static char *getChallenge (int fd)
{
  static char buf [1024] ;
  int num ;

  for (;;)
  {
    if ((num = remoteReadline (fd, buf, 1023)) < 0)
      return NULL ;
    buf [num] = 0 ;

    if (strncmp (buf, "Challenge ", 10) == 0)
      return &buf [10] ;
  }
}


/*
 * authenticate:
 *	Read in the challenge from the server, use it to encrypt our password
 *	and send it back to the server. Wait for a reply back from the server
 *	to say that we're good to go.
 *	The server will simply disconnect on a bad response. No 3 chances here.
 *********************************************************************************
 */

static int authenticate (int fd, const char *pass)
{
  char *challenge ;
  char *encrypted ;
  char salted [1024] ;

  if ((challenge = getChallenge (fd)) == NULL)
    return -1 ;

  sprintf (salted, "$6$%s$", challenge) ;
  encrypted = crypt (pass, salted) ;
  
// This is an assertion, or sanity check on my part...
//	The '20' comes from the $6$ then the 16 characters of the salt,
//	then the terminating $.

  if (strncmp (encrypted, salted, 20) != 0)
  {
    errno = EBADE ;
    return -1 ;
  }

// 86 characters is the length of the SHA-256 hash

  if (write (fd, encrypted + 20, 86) == 86)
    return 0 ;
  else
    return -1 ;
}


/*
 * _drcSetupNet:
 *	Do the hard work of establishing a network connection and authenticating
 *	the password.
 *********************************************************************************
 */

int _drcSetupNet (const char *ipAddress, const char *port, const char *password)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp ;
  struct in6_addr serveraddr ;
  int remoteFd ;

// Start by seeing if we've been given a (textual) numeric IP address
//	which will save lookups in getaddrinfo()

  memset (&hints, 0, sizeof (hints)) ;
  hints.ai_flags    = AI_NUMERICSERV ;
  hints.ai_family   = AF_UNSPEC ;
  hints.ai_socktype = SOCK_STREAM ;
  hints.ai_protocol = 0 ;

  if (inet_pton (AF_INET, ipAddress, &serveraddr) == 1)		// Valid IPv4
  {
    hints.ai_family = AF_INET ;
    hints.ai_flags |= AI_NUMERICHOST ;
  }
  else
  {
    if (inet_pton (AF_INET6, ipAddress, &serveraddr) == 1)	// Valid IPv6
    {
      hints.ai_family = AF_INET6 ;
      hints.ai_flags |= AI_NUMERICHOST ;
    }
  }

// Now use getaddrinfo() with the newly supplied hints

  if (getaddrinfo (ipAddress, port, &hints, &result) != 0)
    return -1 ;

// Now try each address in-turn until we get one that connects...

  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
    if ((remoteFd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0)
      continue ;

    if (connect (remoteFd, rp->ai_addr, rp->ai_addrlen) < 0)
      continue ;

    if (authenticate (remoteFd, password) < 0)
    {
      close (remoteFd) ;
      errno = EACCES ;		// Permission denied
      return -1 ;
    }
    else
      return remoteFd ;
  }

  errno = EHOSTUNREACH ;	// Host unreachable - may not be right, but good enough
  return -1 ; // Nothing connected
}


/*
 * myPinMode:
 *	Change the pin mode on the remote DRC device
 *********************************************************************************
 */

static void myPinMode (struct wiringPiNodeStruct *node, int pin, int mode)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_PIN_MODE ;
  cmd.data = mode ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}


/*
 * myPullUpDnControl:
 *********************************************************************************
 */

static void myPullUpDnControl (struct wiringPiNodeStruct *node, int pin, int mode)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_PULL_UP_DN ;
  cmd.data = mode ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}


/*
 * myDigitalWrite:
 *********************************************************************************
 */

static void myDigitalWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_DIGITAL_WRITE ;
  cmd.data = value ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}


/*
 * myDigitalWrite8:
 *********************************************************************************

static void myDigitalWrite8 (struct wiringPiNodeStruct *node, int pin, int value)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_DIGITAL_WRITE8 ;
  cmd.data = value ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}
 */


/*
 * myAnalogWrite:
 *********************************************************************************
 */

static void myAnalogWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_ANALOG_WRITE ;
  cmd.data = value ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}


/*
 * myPwmWrite:
 *********************************************************************************
 */

static void myPwmWrite (struct wiringPiNodeStruct *node, int pin, int value)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_PWM_WRITE ;
  cmd.data = value ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;
}


/*
 * myAnalogRead:
 *********************************************************************************
 */

static int myAnalogRead (struct wiringPiNodeStruct *node, int pin)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_ANALOG_READ ;
  cmd.data = 0 ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;

  return cmd.data ;
}


/*
 * myDigitalRead:
 *********************************************************************************
 */

static int myDigitalRead (struct wiringPiNodeStruct *node, int pin)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_DIGITAL_READ ;
  cmd.data = 0 ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;

  return cmd.data ;
}


/*
 * myDigitalRead8:
 *********************************************************************************

static unsigned int myDigitalRead8 (struct wiringPiNodeStruct *node, int pin)
{
  struct drcNetComStruct cmd ;

  cmd.pin  = pin - node->pinBase ;
  cmd.cmd  = DRCN_DIGITAL_READ8 ;
  cmd.data = 0 ;

  (void)send (node->fd, &cmd, sizeof (cmd), 0) ;
  (void)recv (node->fd, &cmd, sizeof (cmd), 0) ;

  return cmd.data ;
}
 */


/*
 * drcNet:
 *	Create a new instance of an DRC GPIO interface.
 *	Could be a variable nunber of pins here - we might not know in advance.
 *********************************************************************************
 */

int drcSetupNet (const int pinBase, const int numPins, const char *ipAddress, const char *port, const char *password)
{
  int fd, len ;
  struct wiringPiNodeStruct *node ;

  if ((fd = _drcSetupNet (ipAddress, port, password)) < 0)
    return FALSE ;

  len = sizeof (struct drcNetComStruct) ;

  if (setsockopt (fd, SOL_SOCKET, SO_RCVLOWAT, (void *)&len, sizeof (len)) < 0)
    return FALSE ;

  node = wiringPiNewNode (pinBase, numPins) ;

  node->fd               = fd ;
  node->pinMode          = myPinMode ;
  node->pullUpDnControl  = myPullUpDnControl ;
  node->analogRead       = myAnalogRead ;
  node->analogRead       = myAnalogRead ;
  node->analogWrite      = myAnalogWrite ;
  node->digitalRead      = myDigitalRead ;
  node->digitalWrite     = myDigitalWrite ;
//node->digitalRead8     = myDigitalRead8 ;
//node->digitalWrite8    = myDigitalWrite8 ;
  node->pwmWrite         = myPwmWrite ;

  return TRUE ;
}
