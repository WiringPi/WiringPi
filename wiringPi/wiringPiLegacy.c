
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