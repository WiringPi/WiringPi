/*
 * wiringPiSPI.c:
 *	Simplified SPI access routines
 *	Copyright (c) 2012 Gordon Henderson
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


#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "wiringPi.h"

#include "wiringPiSPI.h"


// The SPI bus parameters
//	Variables as they need to be passed as pointers later on

const static char       *spiDev0  = "/dev/spidev0.0" ;
const static char       *spiDev1  = "/dev/spidev0.1" ;
const static uint8_t     spiMode  = 0 ;
const static uint8_t     spiBPW   = 8 ;
const static uint16_t    spiDelay = 0 ;

static uint32_t    spiSpeeds [2] ;
static int         spiFds [2] ;


/*
 * wiringPiSPIGetFd:
 *	Return the file-descriptor for the given channel
 *********************************************************************************
 */

int wiringPiSPIGetFd (int channel)
{
  return spiFds [channel & 1] ;
}


/*
 * wiringPiSPIDataRW:
 *	Write and Read a block of data over the SPI bus.
 *	Note the data ia being read into the transmit buffer, so will
 *	overwrite it!
 *	This is also a full-duplex operation.
 *********************************************************************************
 */

int wiringPiSPIDataRW (int channel, unsigned char *data, int len)
{
  struct spi_ioc_transfer spi ;

  channel &= 1 ;

  spi.tx_buf        = (unsigned long)data ;
  spi.rx_buf        = (unsigned long)data ;
  spi.len           = len ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeeds [channel] ;
  spi.bits_per_word = spiBPW ;

  return ioctl (spiFds [channel], SPI_IOC_MESSAGE(1), &spi) ;
}


/*
 * wiringPiSPISetup:
 *	Open the SPI device, and set it up, etc.
 *********************************************************************************
 */

int wiringPiSPISetup (int channel, int speed)
{
  int fd ;

  channel &= 1 ;

  if ((fd = open (channel == 0 ? spiDev0 : spiDev1, O_RDWR)) < 0)
    return wiringPiFailure (WPI_ALMOST, "Unable to open SPI device: %s\n", strerror (errno)) ;

  spiSpeeds [channel] = speed ;
  spiFds    [channel] = fd ;

// Set SPI parameters.
//	Why are we reading it afterwriting it? I've no idea, but for now I'm blindly
//	copying example code I've seen online...

  if (ioctl (fd, SPI_IOC_WR_MODE, &spiMode)         < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI Mode Change failure: %s\n", strerror (errno)) ;
  
  if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI BPW Change failure: %s\n", strerror (errno)) ;

  if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI Speed Change failure: %s\n", strerror (errno)) ;

  return fd ;
}
