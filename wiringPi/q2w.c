/*
 * q2w.c:
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

// MCP23S17 Registers

#define IOCON           0x0A

#define IODIRA          0x00
#define IPOLA           0x02
#define GPINTENA        0x04
#define DEFVALA         0x06
#define INTCONA         0x08
#define GPPUA           0x0C
#define INTFA           0x0E
#define INTCAPA         0x10
#define GPIOA           0x12
#define OLATA           0x14

#define IODIRB          0x01
#define IPOLB           0x03
#define GPINTENB        0x05
#define DEFVALB         0x07
#define INTCONB         0x09
#define GPPUB           0x0D
#define INTFB           0x0F
#define INTCAPB         0x11
#define GPIOB           0x13
#define OLATB           0x15

// Bits in the IOCON register

#define IOCON_BANK_MODE 0x80
#define IOCON_MIRROR    0x40
#define IOCON_SEQOP     0x20
#define IOCON_DISSLW    0x10
#define IOCON_HAEN      0x08
#define IOCON_ODR       0x04
#define IOCON_INTPOL    0x02
#define IOCON_UNUSED    0x01

// Default initialisation mode

#define IOCON_INIT      (IOCON_SEQOP)



/*
 *********************************************************************************
 * The works
 *********************************************************************************
 */

int main (int argc, char *argv [])
{
  int q2w ;

//  if (wiringPiSetup () == -1)
//    { fprintf (stderr, "q2w: Unable to initialise wiringPi: %s\n", strerror (errno)) ; return 1 ; }

  if ((q2w = wiringPiI2CSetup (0x20)) == -1)
    { fprintf (stderr, "q2w: Unable to initialise I2C: %s\n", strerror (errno)) ; return 1 ; }

// Very simple direct control of the MCP23017:

  wiringPiI2CWriteReg8 (q2w, IOCON, IOCON_INIT) ;
  wiringPiI2CWriteReg8 (q2w, IODIRA, 0x00) ;		// Port A -> Outputs
  wiringPiI2CWriteReg8 (q2w, IODIRB, 0x00) ;		// Port B -> Outputs

  for (;;)
  {
    wiringPiI2CWriteReg8 (q2w, GPIOA, 0x00) ;		// All Off
    delay (500) ;
    wiringPiI2CWriteReg8 (q2w, GPIOA, 0xFF) ;		// All On
    delay (500) ;
  }
  
  return 0 ;
}
