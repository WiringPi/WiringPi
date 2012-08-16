/*
 * gertboard.c:
 *	Access routines for the SPI devices on the Gertboard
 *	Copyright (c) 2012 Gordon Henderson
 *
 *	The Gertboard has:
 *
 *		An MCP3002 dual-channel A to D convertor connected
 *		to the SPI bus, selected by chip-select A, and:
 *
 *		An MCP4802 dual-channel D to A convertor connected
 *		to the SPI bus, selected via chip-select B.
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
#include <linux/spi/spidev.h>

#include "gertboard.h"


// The SPI bus parameters
//	Variables as they need to be passed as pointers later on

static char       *spiA2D = "/dev/spidev0.0" ;
static char       *spiD2A = "/dev/spidev0.1" ;
static uint8_t     spiMode   = 0 ;
static uint8_t     spiBPW    = 8 ;
static uint32_t    spiSpeed  = 100000 ;	// 1MHz
static uint16_t    spiDelay  = 0;

// Locals here to keep track of everything

static int spiFdA2D ;
static int spiFdD2A ;


/*
 * gertboardAnalogWrite:
 *	Write an 8-bit data value to the MCP4802 Analog to digital
 *	convertor on the Gertboard.
 *********************************************************************************
 */

void gertboardAnalogWrite (int chan, int value)
{
  uint8_t spiBufTx [2] ;
  uint8_t spiBufRx [2] ;
  struct spi_ioc_transfer spi ;

  uint8_t chanBits, dataBits ;

  if (chan == 0)
    chanBits = 0x30 ;
  else
    chanBits = 0xB0 ;

  chanBits |= ((value >> 4) & 0x0F) ;
  dataBits  = ((value << 4) & 0xF0) ;

  spiBufTx [0] = chanBits ;
  spiBufTx [1] = dataBits ;

  spi.tx_buf        = (unsigned long)spiBufTx ;
  spi.rx_buf        = (unsigned long)spiBufRx ;
  spi.len           = 2 ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeed ;
  spi.bits_per_word = spiBPW ;

  ioctl (spiFdD2A, SPI_IOC_MESSAGE(1), &spi) ;
}


/*
 * gertboardAnalogRead:
 *	Return the analog value of the given channel (0/1).
 *	The A/D is a 10-bit device
 *********************************************************************************
 */

int gertboardAnalogRead (int chan)
{
  uint8_t spiBufTx [4] ;
  uint8_t spiBufRx [4] ;
  struct spi_ioc_transfer spi ;

  uint8_t chanBits ;

  if (chan == 0)
    chanBits = 0b0110100 ;
  else
    chanBits = 0b0111100 ;

  spiBufTx [0] = chanBits ;
  spiBufTx [1] = 0 ;

  spi.tx_buf        = (unsigned long)spiBufTx ;
  spi.rx_buf        = (unsigned long)spiBufRx ;
  spi.len           = 4 ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeed ;
  spi.bits_per_word = spiBPW ;

  ioctl (spiFdA2D, SPI_IOC_MESSAGE(1), &spi) ;

  return spiBufRx [0] << 8 | spiBufRx [1] ;
}


/*
 * setParams:
 *	Output the SPI bus parameters to the given device
 *********************************************************************************
 */

static int setParams (int fd)
{
  if (ioctl (fd, SPI_IOC_WR_MODE, &spiMode) < 0)
    return -1 ;

  if (ioctl (fd, SPI_IOC_RD_MODE, &spiMode) < 0)
    return -1 ;

  if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    return -1 ;

  if (ioctl (fd, SPI_IOC_RD_BITS_PER_WORD, &spiBPW) < 0)
    return -1 ;

  if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiSpeed) < 0)
    return -1 ;

  if (ioctl (fd, SPI_IOC_RD_MAX_SPEED_HZ, &spiSpeed) < 0)
    return -1 ;

  return 0 ;
}


/*
 * gertboardSPISetup:
 *	Initialise the SPI bus, etc.
 *********************************************************************************
 */

int gertboardSPISetup (void)
{
  if ((spiFdA2D = open (spiA2D, O_RDWR)) < 0)
    return -1 ;

  if (setParams (spiFdA2D) != 0)
    return -1 ;

  if ((spiFdD2A = open (spiD2A, O_RDWR)) < 0)
    return -1 ;

  if (setParams (spiFdD2A) != 0)
    return -1 ;

  return 0 ;
}
