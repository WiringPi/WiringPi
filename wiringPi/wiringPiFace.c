/*
 * wiringPiFace:
 *	Arduino compatable (ish) Wiring library for the Raspberry Pi
 *	Copyright (c) 2012 Gordon Henderson
 *
 *	This file to interface with the PiFace peripheral device which
 *	has an MCP23S17 GPIO device connected via the SPI bus.
 *
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/spi/spidev.h>

#include "wiringPi.h"


// The SPI bus parameters
//	Variables as they need to be passed as pointers later on

static char       *spiDevice = "/dev/spidev0.0" ;
static uint8_t     spiMode   = 0 ;
static uint8_t     spiBPW    = 8 ;
static uint32_t    spiSpeed  = 5000000 ;
static uint16_t    spiDelay  = 0;

// Locals here to keep track of everything

static int spiFd ;

// The MCP23S17 doesn't have bit-set operations, so it's
//	cheaper to keep a copy here than to read/modify/write it

uint8_t dataOutRegister = 0 ;
uint8_t     pudRegister = 0 ;

// MCP23S17 Registers

#define	IOCON		0x0A

#define	IODIRA		0x00
#define	IPOLA		0x02
#define	GPINTENA	0x04
#define	DEFVALA		0x06
#define	INTCONA		0x08
#define	GPPUA		0x0C
#define	INTFA		0x0E
#define	INTCAPA		0x10
#define	GPIOA		0x12
#define	OLATA		0x14

#define	IODIRB		0x01
#define	IPOLB		0x03
#define	GPINTENB	0x05
#define	DEFVALB		0x07
#define	INTCONB		0x09
#define	GPPUB		0x0D
#define	INTFB		0x0F
#define	INTCAPB		0x11
#define	GPIOB		0x13
#define	OLATB		0x15

// Bits in the IOCON register

#define	IOCON_BANK_MODE	0x80
#define	IOCON_MIRROR	0x40
#define	IOCON_SEQOP	0x20
#define	IOCON_DISSLW	0x10
#define	IOCON_HAEN	0x08
#define	IOCON_ODR	0x04
#define	IOCON_INTPOL	0x02
#define	IOCON_UNUSED	0x01

// Default initialisation mode

#define	IOCON_INIT	(IOCON_SEQOP)

// Command codes

#define	CMD_WRITE	0x40
#define CMD_READ	0x41


/*
 * writeByte:
 *	Write a byte to a register on the MCP23S17 on the SPI bus.
 *	This is using the synchronous access mechanism.
 *********************************************************************************
 */

static void writeByte (uint8_t reg, uint8_t data)
{
  uint8_t spiBufTx [3] ;
  uint8_t spiBufRx [3] ;
  struct spi_ioc_transfer spi ;

  spiBufTx [0] = CMD_WRITE ;
  spiBufTx [1] = reg ;
  spiBufTx [2] = data ;

  spi.tx_buf        = (unsigned long)spiBufTx ;
  spi.rx_buf        = (unsigned long)spiBufRx ;
  spi.len           = 3 ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeed ;
  spi.bits_per_word = spiBPW ;

  ioctl (spiFd, SPI_IOC_MESSAGE(1), &spi) ;
}

/*
 * readByte:
 *	Read a byte from a register on the MCP23S17 on the SPI bus.
 *	This is the synchronous access mechanism.
 *	What appears to happen is that the data returned is at
 *	the same offset as the number of bytes written to the device. So if we
 *	write 2 bytes (e.g. command then register number), then the data returned
 *	will by at the 3rd byte...
 *********************************************************************************
 */

static uint8_t readByte (uint8_t reg)
{
  uint8_t tx [4] ;
  uint8_t rx [4] ;
  struct spi_ioc_transfer spi ;

  tx [0] = CMD_READ ;
  tx [1] = reg ;
  tx [2] = 0 ;

  spi.tx_buf        = (unsigned long)tx ;
  spi.rx_buf        = (unsigned long)rx ;
  spi.len           = 3 ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeed ;
  spi.bits_per_word = spiBPW ;

  ioctl (spiFd, SPI_IOC_MESSAGE(1), &spi) ;

  return rx [2] ;
}


/*
 * digitalWritePiFace:
 *	Perform the digitalWrite function on the PiFace board
 *********************************************************************************
 */

void digitalWritePiFace (int pin, int value)
{
  uint8_t mask = 1 << pin ;

  if (value == 0)
    dataOutRegister &= (~mask) ;
  else
    dataOutRegister |=   mask ;

  writeByte (GPIOA, dataOutRegister) ;
}


void digitalWritePiFaceSpecial (int pin, int value)
{
  uint8_t mask = 1 << pin ;
  uint8_t old ;

  old = readByte (GPIOA) ;

  if (value == 0)
    old &= (~mask) ;
  else
    old |=   mask ;

  writeByte (GPIOA, old) ;
}


/*
 * digitalReadPiFace:
 *	Perform the digitalRead function on the PiFace board
 *********************************************************************************
 */

int digitalReadPiFace (int pin)
{
  uint8_t mask = 1 << pin ;

  if ((readByte (GPIOB) & mask) != 0)
    return HIGH ;
  else
    return LOW ;
}


/*
 * pullUpDnControlPiFace:
 *	Perform the pullUpDnControl function on the PiFace board
 *********************************************************************************
 */

void pullUpDnControlPiFace (int pin, int pud)
{
  uint8_t mask = 1 << pin ;

  if (pud == PUD_UP)
    pudRegister |=   mask ;
  else
    pudRegister &= (~mask) ;

  writeByte (GPPUB, pudRegister) ;

}


void pullUpDnControlPiFaceSpecial (int pin, int pud)
{
  uint8_t mask = 1 << pin ;
  uint8_t old ;

  old = readByte (GPPUB) ;

  if (pud == PUD_UP)
    old |=   mask ;
  else
    old &= (~mask) ;

  writeByte (GPPUB, old) ;

}



/*
 * Dummy functions that are not used in this mode
 *********************************************************************************
 */

void pinModePiFace          (int pin, int mode)  {}
void pwmWritePiFace         (int pin, int value) {}
int  waitForInterruptPiFace (int pin, int mS)    { return 0 ; }


/*
 * wiringPiSetupPiFace
 *	Setup the SPI interface and initialise the MCP23S17 chip
 *********************************************************************************
 */

static int _wiringPiSetupPiFace (void)
{
  if ((spiFd = open (spiDevice, O_RDWR)) < 0)
    return -1 ;

// Set SPI parameters
//	Why are we doing a read after write?
//	I don't know - just blindliy copying an example elsewhere... -GH-

  if (ioctl (spiFd, SPI_IOC_WR_MODE, &spiMode) < 0)
    return -1 ;

  if (ioctl (spiFd, SPI_IOC_RD_MODE, &spiMode) < 0)
    return -1 ;

  if (ioctl (spiFd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    return -1 ;

  if (ioctl (spiFd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW) < 0)
    return -1 ;

  if (ioctl (spiFd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed) < 0)
    return -1 ;

  if (ioctl (spiFd, SPI_IOC_RD_MAX_SPEED_HZ, &spiSpeed) < 0)
    return -1 ;

// Setup the MCP23S17

  writeByte (IOCON, IOCON_INIT) ;

  writeByte (IODIRA, 0x00) ;	// Port A -> Outputs
  writeByte (IODIRB, 0xFF) ;	// Port B -> Inputs

  return 0 ;
}


int wiringPiSetupPiFace (void)
{
  int x = _wiringPiSetupPiFace () ;

  if (x != 0)
    return x ;

  writeByte (GPIOA, 0x00) ;	// Set all outptus off
  writeByte (GPPUB, 0x00) ;	// Disable any pull-ups on port B

           pinMode =          pinModePiFace ;
   pullUpDnControl =  pullUpDnControlPiFace ;
      digitalWrite =     digitalWritePiFace ;
          pwmWrite =         pwmWritePiFace ;
       digitalRead =      digitalReadPiFace ;
  waitForInterrupt = waitForInterruptPiFace ;

  return 0 ;
}


/*
 * wiringPiSetupPiFaceForGpioProg:
 *	Setup the SPI interface and initialise the MCP23S17 chip
 *	Special version for the gpio program
 *********************************************************************************
 */


int wiringPiSetupPiFaceForGpioProg (void)
{
  int x = _wiringPiSetupPiFace () ;

  if (x != 0)
    return x ;

           pinMode =          pinModePiFace ;
   pullUpDnControl =  pullUpDnControlPiFaceSpecial ;
      digitalWrite =     digitalWritePiFaceSpecial ;
          pwmWrite =         pwmWritePiFace ;
       digitalRead =      digitalReadPiFace ;
  waitForInterrupt = waitForInterruptPiFace ;

  return 0 ;
}
