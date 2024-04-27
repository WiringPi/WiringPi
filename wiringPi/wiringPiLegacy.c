
#include <wiringPi.h>
#include "wiringPiLegacy.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern int wiringPiDebug ;

const char* GetPiRevisionLegacy(char* line, int linelength, unsigned int* revision) {
  FILE *cpuFd ;
  char *c = NULL;

  if ((cpuFd = fopen ("/proc/cpuinfo", "r")) == NULL)
    piGpioLayoutOops ("Unable to open /proc/cpuinfo") ;

  while (fgets (line, linelength, cpuFd) != NULL) 
    if (strncmp (line, "Revision", 8) == 0)
      break ;

  fclose (cpuFd) ;

  if (strncmp (line, "Revision", 8) != 0)
    piGpioLayoutOops ("No \"Revision\" line") ;

// Chomp trailing CR/NL

  for (c = &line [strlen (line) - 1] ; (*c == '\n') || (*c == '\r') ; --c)
    *c = 0 ;

  if (wiringPiDebug)
    printf ("GetPiRevisionLegacy: Revision string: %s\n", line) ;

// Need to work out if it's using the new or old encoding scheme:

// Scan to the first character of the revision number

  for (c = line ; *c ; ++c)
    if (*c == ':')
      break ;

  if (*c != ':')
    piGpioLayoutOops ("Bogus \"Revision\" line (no colon)") ;

// Chomp spaces

  ++c ;
  while (isspace (*c))
    ++c ;

  if (!isxdigit (*c))
    piGpioLayoutOops ("Bogus \"Revision\" line (no hex digit at start of revision)") ;

  *revision = (unsigned int)strtol (c, NULL, 16) ; // Hex number with no leading 0x
  return c;
}



// useless
int piGpioLayoutLegacy (void)
{
  FILE *cpuFd ;
  char line [120] ;
  char *c ;
  static int  gpioLayout = -1 ;

  if (gpioLayout != -1)	// No point checking twice
    return gpioLayout ;

  if ((cpuFd = fopen ("/proc/cpuinfo", "r")) == NULL)
    piGpioLayoutOops ("Unable to open /proc/cpuinfo") ;

#ifdef	DONT_CARE_ANYMORE
// Start by looking for the Architecture to make sure we're really running
//	on a Pi. I'm getting fed-up with people whinging at me because
//	they can't get it to work on weirdFruitPi boards...
  while (fgets (line, 120, cpuFd) != NULL)
    if (strncmp (line, "Hardware", 8) == 0)
      break ;

  if (strncmp (line, "Hardware", 8) != 0)
    piGpioLayoutOops ("No \"Hardware\" line") ;

  if (wiringPiDebug)
    printf ("piGpioLayout: Hardware: %s\n", line) ;

// See if it's BCM2708 or BCM2709 or the new BCM2835.

// OK. As of Kernel 4.8,  we have BCM2835 only, regardless of model.
//	However I still want to check because it will trap the cheapskates and rip-
//	off merchants who want to use wiringPi on non-Raspberry Pi platforms - which
//	I do not support so don't email me your bleating whinges about anything
//	other than a genuine Raspberry Pi.

  if (! (strstr (line, "BCM2708") || strstr (line, "BCM2709") || strstr (line, "BCM2835")))
  {
    fprintf (stderr, "Unable to determine hardware version. I see: %s,\n", line) ;
    fprintf (stderr, " - expecting BCM2708, BCM2709 or BCM2835.\n") ;
    fprintf (stderr, "If this is a genuine Raspberry Pi then please report this\n") ;
    fprintf (stderr, "at GitHub.com/wiringPi/wiringPi. If this is not a Raspberry Pi then you\n") ;
    fprintf (stderr, "are on your own as wiringPi is designed to support the\n") ;
    fprintf (stderr, "Raspberry Pi ONLY.\n") ;
    exit (EXIT_FAILURE) ;
  }

// Actually... That has caused me more than 10,000 emails so-far. Mosty by
//	people who think they know better by creating a statically linked
//	version that will not run with a new 4.9 kernel. I utterly hate and
//	despise those people.
//
//	I also get bleats from people running other than Raspbian with another
//	distros compiled kernel rather than a foundation compiled kernel, so
//	this might actually help them. It might not - I only have the capacity
//	to support Raspbian.
//
//	However, I've decided to leave this check out and rely purely on the
//	Revision: line for now. It will not work on a non-pi hardware or weird
//	kernels that don't give you a suitable revision line.

// So - we're Probably on a Raspberry Pi. Check the revision field for the real
//	hardware type
//	In-future, I ought to use the device tree as there are now Pi entries in
//	/proc/device-tree/ ...
//	but I'll leave that for the next revision. Or the next.
#endif

// Isolate the Revision line

  rewind (cpuFd) ;
  while (fgets (line, 120, cpuFd) != NULL)
    if (strncmp (line, "Revision", 8) == 0)
      break ;

  fclose (cpuFd) ;

  if (strncmp (line, "Revision", 8) != 0)
    piGpioLayoutOops ("No \"Revision\" line") ;

// Chomp trailing CR/NL

  for (c = &line [strlen (line) - 1] ; (*c == '\n') || (*c == '\r') ; --c)
    *c = 0 ;

  if (wiringPiDebug)
    printf ("piGpioLayout: Revision string: %s\n", line) ;

// Scan to the first character of the revision number

  for (c = line ; *c ; ++c)
    if (*c == ':')
      break ;

  if (*c != ':')
    piGpioLayoutOops ("Bogus \"Revision\" line (no colon)") ;

// Chomp spaces

  ++c ;
  while (isspace (*c))
    ++c ;

  if (!isxdigit (*c))
    piGpioLayoutOops ("Bogus \"Revision\" line (no hex digit at start of revision)") ;

// Make sure its long enough

  if (strlen (c) < 4)
    piGpioLayoutOops ("Bogus revision line (too small)") ;

// Isolate  last 4 characters: (in-case of overvolting or new encoding scheme)

  c = c + strlen (c) - 4 ;

  if (wiringPiDebug)
    printf ("piGpioLayout: last4Chars are: \"%s\"\n", c) ;

  if ( (strcmp (c, "0002") == 0) || (strcmp (c, "0003") == 0))
    gpioLayout = 1 ;
  else
    gpioLayout = 2 ;	// Covers everything else from the B revision 2 to the B+, the Pi v2, v3, zero and CM's.

  if (wiringPiDebug)
    printf ("piGpioLayout: Returning revision: %d\n", gpioLayout) ;

  return gpioLayout ;
}
