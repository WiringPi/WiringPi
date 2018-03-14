/*
 * extensions.c:
 *	Originally part of the GPIO program to test, peek, poke and otherwise
 *	noodle with the GPIO hardware on the Raspberry Pi.
 *	Now used as a general purpose library to allow systems to dynamically
 *	add in new devices into wiringPi at program run-time.
 *	Copyright (c) 2012-2015 Gordon Henderson
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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

#include <wiringPi.h>

#include "mcp23008.h"
#include "mcp23016.h"
#include "mcp23017.h"
#include "mcp23s08.h"
#include "mcp23s17.h"
#include "sr595.h"
#include "pcf8574.h"
#include "pcf8591.h"
#include "mcp3002.h"
#include "mcp3004.h"
#include "mcp4802.h"
#include "mcp3422.h"
#include "max31855.h"
#include "max5322.h"
#include "ads1115.h"
#include "sn3218.h"
#include "drcSerial.h"
#include "drcNet.h"
#include "../wiringPiD/drcNetCmd.h"
#include "pseudoPins.h"
#include "bmp180.h"
#include "htu21d.h"
#include "ds18b20.h"
#include "rht03.h"

#include "wpiExtensions.h"

extern int wiringPiDebug ;

static int verbose ;
static char errorMessage [1024] ;


// Local structure to hold details

struct extensionFunctionStruct
{
  const char *name ;
  int	(*function)(char *progName, int pinBase, char *params) ;
} ;


/*
 * verbError:
 *	Convenient error handling
 *********************************************************************************
 */

static void verbError (const char *message, ...)
{
  va_list argp ;
  va_start (argp, message) ;
    vsnprintf (errorMessage, 1023, message, argp) ;
  va_end (argp) ;

  if (verbose)
    fprintf (stderr, "%s\n", errorMessage) ;
}


/*
 * extractInt:
 *	Check & return an integer at the given location (prefixed by a :)
 *********************************************************************************
 */

static char *extractInt (char *progName, char *p, int *num)
{
  if (*p != ':')
  {
    verbError ("%s: colon expected", progName) ;
    return NULL ;
  }

  ++p ;

  if (!isdigit (*p))
  {
    verbError ("%s: digit expected", progName) ;
    return NULL ;
  }

  *num = strtol (p, NULL, 0) ;

// Increment p, but we need to check for hex 0x

  if ((*p == '0') && (*(p + 1) == 'x'))
    p +=2 ;

  while (isxdigit (*p))
    ++p ;

  return p ;
}


/*
 * extractStr:
 *	Check & return a string at the given location (prefixed by a :)
 *	Note: The string can be enclosed in []'s to escape colons. This is
 *	so we can handle IPv6 addresses which contain colons and the []'s is
 *	a common way to prepresent them.
 *********************************************************************************
 */

static char *extractStr (char *progName, char *p, char **str)
{
  char *q, *r ;
  int quoted = FALSE ;

  if (*p != ':')
  {
    verbError ("%s: colon expected", progName) ;
    return NULL ;
  }

  ++p ;

  if (*p == '[')
  {
    quoted = TRUE ;
    ++p ;
  }

  if (!isprint (*p))	// Is this needed?
  {
    verbError ("%s: character expected", progName) ;
    return NULL ;
  }

  q = p ;
  if (quoted)
  {
    while ((*q != 0) && (*q != ']'))
      ++q ;
  }
  else
  {
    while ((*q != 0) && (*q != ':'))
      ++q ;
  }

  *str = r = calloc (q - p + 2, 1) ;	// Zeros it

  while (p != q)
    *r++ = *p++ ;

  if (quoted)				// Skip over the ] to the :
    ++p ;

  return p ;
}



/*
 * doExtensionMcp23008:
 *	MCP23008 - 8-bit I2C GPIO expansion chip
 *	mcp23002:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionMcp23008 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x01) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  mcp23008Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionMcp23016:
 *	MCP230016- 16-bit I2C GPIO expansion chip
 *	mcp23016:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionMcp23016 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  mcp23016Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionMcp23017:
 *	MCP230017- 16-bit I2C GPIO expansion chip
 *	mcp23017:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionMcp23017 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  mcp23017Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionMcp23s08:
 *	MCP23s08 - 8-bit SPI GPIO expansion chip
 *	mcp23s08:base:spi:port
 *********************************************************************************
 */

static int doExtensionMcp23s08 (char *progName, int pinBase, char *params)
{
  int spi, port ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI address (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &port)) == NULL)
    return FALSE ;

  if ((port < 0) || (port > 7))
  {
    verbError ("%s: port address (%d) out of range", progName, port) ;
    return FALSE ;
  }

  mcp23s08Setup (pinBase, spi, port) ;

  return TRUE ;
}


/*
 * doExtensionMcp23s17:
 *	MCP23s17 - 16-bit SPI GPIO expansion chip
 *	mcp23s17:base:spi:port
 *********************************************************************************
 */

static int doExtensionMcp23s17 (char *progName, int pinBase, char *params)
{
  int spi, port ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI address (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &port)) == NULL)
    return FALSE ;

  if ((port < 0) || (port > 7))
  {
    verbError ("%s: port address (%d) out of range", progName, port) ;
    return FALSE ;
  }

  mcp23s17Setup (pinBase, spi, port) ;

  return TRUE ;
}


/*
 * doExtensionSr595:
 *	Shift Register 74x595
 *	sr595:base:pins:data:clock:latch
 *********************************************************************************
 */

static int doExtensionSr595 (char *progName, int pinBase, char *params)
{
  int pins, data, clock, latch ;

// Extract pins

  if ((params = extractInt (progName, params, &pins)) == NULL)
    return FALSE ;

  if ((pins < 8) || (pins > 32))
  {
    verbError ("%s: pin count (%d) out of range - 8-32 expected.", progName, pins) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &data)) == NULL)
    return FALSE ;

  if ((params = extractInt (progName, params, &clock)) == NULL)
    return FALSE ;

  if ((params = extractInt (progName, params, &latch)) == NULL)
    return FALSE ;

  sr595Setup (pinBase, pins, data, clock, latch) ;

  return TRUE ;
}


/*
 * doExtensionPcf8574:
 *	Digital IO (Crude!)
 *	pcf8574:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionPcf8574 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  pcf8574Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionAds1115:
 *	Analog Input
 *	ads1115:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionAds1115 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  ads1115Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionPcf8591:
 *	Analog IO
 *	pcf8591:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionPcf8591 (char *progName, int pinBase, char *params)
{
  int i2c ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  pcf8591Setup (pinBase, i2c) ;

  return TRUE ;
}


/*
 * doExtensionPseudoPins:
 *	64 Memory resident pseudo pins
 *	pseudoPins:base
 *********************************************************************************
 */

static int doExtensionPseudoPins (UNU char *progName, int pinBase, UNU char *params)
{
  pseudoPinsSetup (pinBase) ;

  return TRUE ;
}


/*
 * doExtensionBmp180:
 *	Analog Temp + Pressure
 *	bmp180:base
 *********************************************************************************
 */

static int doExtensionBmp180 (UNU char *progName, int pinBase, UNU char *params)
{
  bmp180Setup (pinBase) ;

  return TRUE ;
}


/*
 * doExtensionHtu21d:
 *	Analog humidity + Pressure
 *	htu21d:base
 *********************************************************************************
 */

static int doExtensionHtu21d (UNU char *progName, int pinBase, UNU char *params)
{
  htu21dSetup (pinBase) ;

  return TRUE ;
}


/*
 * doExtensionDs18b20:
 *	1-Wire Temperature
 *	htu21d:base:serialNum
 *********************************************************************************
 */

static int doExtensionDs18b20 (char *progName, int pinBase, char *params)
{
  char *serialNum ;

  if ((params = extractStr (progName, params, &serialNum)) == NULL)
    return FALSE ;

  return ds18b20Setup (pinBase, serialNum) ;
}


/*
 * doExtensionRht03:
 *	Maxdetect 1-Wire Temperature & Humidity
 *	rht03:base:piPin
 *********************************************************************************
 */

static int doExtensionRht03 (char *progName, int pinBase, char *params)
{
  int piPin ;

  if ((params = extractInt (progName, params, &piPin)) == NULL)
    return FALSE ;

  return rht03Setup (pinBase, piPin) ;
}


/*
 * doExtensionMax31855:
 *	Analog IO
 *	max31855:base:spiChan
 *********************************************************************************
 */

static int doExtensionMax31855 (char *progName, int pinBase, char *params)
{
  int spi ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI channel (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  max31855Setup (pinBase, spi) ;

  return TRUE ;
}


/*
 * doExtensionMcp3002:
 *	Analog IO
 *	mcp3002:base:spiChan
 *********************************************************************************
 */

static int doExtensionMcp3002 (char *progName, int pinBase, char *params)
{
  int spi ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI channel (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  mcp3002Setup (pinBase, spi) ;

  return TRUE ;
}


/*
 * doExtensionMcp3004:
 *	Analog IO
 *	mcp3004:base:spiChan
 *********************************************************************************
 */

static int doExtensionMcp3004 (char *progName, int pinBase, char *params)
{
  int spi ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI channel (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  mcp3004Setup (pinBase, spi) ;

  return TRUE ;
}


/*
 * doExtensionMax5322:
 *	Analog O
 *	max5322:base:spiChan
 *********************************************************************************
 */

static int doExtensionMax5322 (char *progName, int pinBase, char *params)
{
  int spi ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI channel (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  max5322Setup (pinBase, spi) ;

  return TRUE ;
}


/*
 * doExtensionMcp4802:
 *	Analog IO
 *	mcp4802:base:spiChan
 *********************************************************************************
 */

static int doExtensionMcp4802 (char *progName, int pinBase, char *params)
{
  int spi ;

  if ((params = extractInt (progName, params, &spi)) == NULL)
    return FALSE ;

  if ((spi < 0) || (spi > 1))
  {
    verbError ("%s: SPI channel (%d) out of range", progName, spi) ;
    return FALSE ;
  }

  mcp4802Setup (pinBase, spi) ;

  return TRUE ;
}


/*
 * doExtensionSn3218:
 *	Analog Output (LED Driver)
 *	sn3218:base
 *********************************************************************************
 */

static int doExtensionSn3218 (UNU char *progName, int pinBase, UNU char *params)
{
  sn3218Setup (pinBase) ;
  return TRUE ;
}


/*
 * doExtensionMcp3422:
 *	Analog IO
 *	mcp3422:base:i2cAddr
 *********************************************************************************
 */

static int doExtensionMcp3422 (char *progName, int pinBase, char *params)
{
  int i2c, sampleRate, gain ;

  if ((params = extractInt (progName, params, &i2c)) == NULL)
    return FALSE ;

  if ((i2c < 0x03) || (i2c > 0x77))
  {
    verbError ("%s: i2c address (0x%X) out of range", progName, i2c) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &sampleRate)) == NULL)
    return FALSE ;

  if ((sampleRate < 0) || (sampleRate > 3))
  {
    verbError ("%s: sample rate (%d) out of range", progName, sampleRate) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &gain)) == NULL)
    return FALSE ;

  if ((gain < 0) || (gain > 3))
  {
    verbError ("%s: gain (%d) out of range", progName, gain) ;
    return FALSE ;
  }

  mcp3422Setup (pinBase, i2c, sampleRate, gain) ;

  return TRUE ;
}


/*
 * doExtensionDrcS:
 *	Interface to a DRC Serial system
 *	drcs:base:pins:serialPort:baud
 *********************************************************************************
 */

static int doExtensionDrcS (char *progName, int pinBase, char *params)
{
  char *port ;
  int pins, baud ;

  if ((params = extractInt (progName, params, &pins)) == NULL)
    return FALSE ;

  if ((pins < 1) || (pins > 1000))
  {
    verbError ("%s: pins (%d) out of range (2-1000)", progName, pins) ;
    return FALSE ;
  }
  
  if ((params = extractStr (progName, params, &port)) == NULL)
    return FALSE ;

  if (strlen (port) == 0)
  {
    verbError ("%s: serial port device name required", progName) ;
    return FALSE ;
  }

  if ((params = extractInt (progName, params, &baud)) == NULL)
    return FALSE ;

  if ((baud < 1) || (baud > 4000000))
  {
    verbError ("%s: baud rate (%d) out of range", progName, baud) ;
    return FALSE ;
  }

  drcSetupSerial (pinBase, pins, port, baud) ;

  return TRUE ;
}


/*
 * doExtensionDrcNet:
 *	Interface to a DRC Network system
 *	drcn:base:pins:ipAddress:port:password
 *********************************************************************************
 */

static int doExtensionDrcNet (char *progName, int pinBase, char *params)
{
  int pins ;
  char *ipAddress, *port, *password ;
  char pPort [1024] ;

  if ((params = extractInt (progName, params, &pins)) == NULL)
    return FALSE ;

  if ((pins < 1) || (pins > 1000))
  {
    verbError ("%s: pins (%d) out of range (2-1000)", progName, pins) ;
    return FALSE ;
  }
  
  if ((params = extractStr (progName, params, &ipAddress)) == NULL)
    return FALSE ;

  if (strlen (ipAddress) == 0)
  {
    verbError ("%s: ipAddress required", progName) ;
    return FALSE ;
  }

  if ((params = extractStr (progName, params, &port)) == NULL)
    return FALSE ;

  if (strlen (port) == 0)
  {
    sprintf (pPort, "%d", DEFAULT_SERVER_PORT) ;
    port = pPort ;
  }

  if ((params = extractStr (progName, params, &password)) == NULL)
    return FALSE ;

  if (strlen (password) == 0)
  {
    verbError ("%s: password required", progName) ;
    return FALSE ;
  }

  return drcSetupNet (pinBase, pins, ipAddress, port, password) ;
}



/*
 * Function list
 *********************************************************************************
 */

static struct extensionFunctionStruct extensionFunctions [] = 
{
  { "mcp23008",		&doExtensionMcp23008 	},
  { "mcp23016",		&doExtensionMcp23016 	},
  { "mcp23017",		&doExtensionMcp23017 	},
  { "mcp23s08",		&doExtensionMcp23s08 	},
  { "mcp23s17",		&doExtensionMcp23s17 	},
  { "sr595",		&doExtensionSr595	},
  { "pcf8574",		&doExtensionPcf8574	},
  { "pcf8591",		&doExtensionPcf8591	},
  { "bmp180",		&doExtensionBmp180	},
  { "pseudoPins",	&doExtensionPseudoPins	},
  { "htu21d",		&doExtensionHtu21d	},
  { "ds18b20",		&doExtensionDs18b20	},
  { "rht03",		&doExtensionRht03	},
  { "mcp3002",		&doExtensionMcp3002	},
  { "mcp3004",		&doExtensionMcp3004	},
  { "mcp4802",		&doExtensionMcp4802	},
  { "mcp3422",		&doExtensionMcp3422	},
  { "max31855",		&doExtensionMax31855	},
  { "ads1115",		&doExtensionAds1115	},
  { "max5322",		&doExtensionMax5322	},
  { "sn3218",		&doExtensionSn3218	},
  { "drcs",		&doExtensionDrcS	},
  { "drcn",		&doExtensionDrcNet	},
  { NULL,		NULL		 	},
} ;


/*
 * loadWPiExtension:
 *	Load in a wiringPi extension
 *	The extensionData always starts with the name, a colon then the pinBase
 *	number. Other parameters after that are decoded by the module in question.
 *********************************************************************************
 */

int loadWPiExtension (char *progName, char *extensionData, int printErrors)
{
  char *p ;
  char *extension = extensionData ;
  struct extensionFunctionStruct *extensionFn ;
  int pinBase = 0 ;

  verbose = printErrors ;

// Get the extension name by finding the first colon

  p = extension ;
  while (*p != ':')
  {
    if (!*p)	// ran out of characters
    {
      verbError ("%s: extension name not terminated by a colon", progName) ;
      return FALSE ;
    }
    ++p ;
  }
  *p++ = 0 ;

// Simple ATOI code

  if (!isdigit (*p))
  {
    verbError ("%s: decimal pinBase number expected after extension name", progName) ;
    return FALSE ;
  }

  while (isdigit (*p))
  {
    if (pinBase > 2147483647) // 2^31-1 ... Lets be realistic here...
    {
      verbError ("%s: pinBase too large", progName) ;
      return FALSE ;
    }

    pinBase = pinBase * 10 + (*p - '0') ;
    ++p ;
  }

  if (pinBase < 64)
  {
    verbError ("%s: pinBase (%d) too small. Minimum is 64.", progName, pinBase) ;
    return FALSE ;
  }

// Search for extensions:

  for (extensionFn = extensionFunctions ; extensionFn->name != NULL ; ++extensionFn)
  {
    if (strcmp (extensionFn->name, extension) == 0)
      return extensionFn->function (progName, pinBase, p) ;
  }

  fprintf (stderr, "%s: extension %s not found", progName, extension) ;
  return FALSE ;
}
