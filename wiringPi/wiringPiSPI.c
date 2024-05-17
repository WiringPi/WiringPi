/*
 * wiringPiSPI.c:
 *	Simplified SPI access routines
 *	Copyright (c) 2012-2015 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <linux/spi/spidev.h>
#include "wiringPi.h"
#include "wiringPiSPI.h"


// The SPI bus parameters
//	Variables as they need to be passed as pointers later on

//static const char       *spiDev0  = "/dev/spidev0.0" ;
//static const char       *spiDev1  = "/dev/spidev0.1" ;
static const uint8_t     spiBPW   = 8 ;
static const uint16_t    spiDelay = 0 ;
//https://datasheets.raspberrypi.com/cm4/cm4-datasheet.pdf
const uint8_t     WPI_MaxSPINumbers   = 7 ;
const uint8_t     WPI_MaxSPIChannels  = 3 ;


static uint32_t    spiSpeeds [7][3] =
{
 {0, 0, 0},
 {0, 0, 0},
 {0, 0, 0},
 {0, 0, 0},
 {0, 0, 0},
 {0, 0, 0},
 {0, 0, 0},
};

static int         spiFds [7][3] =
{
 {-1, -1, -1},
 {-1, -1, -1},
 {-1, -1, -1},
 {-1, -1, -1},
 {-1, -1, -1},
 {-1, -1, -1},
 {-1, -1, -1},
};


int SPICheckLimits(const int number, const int channel) {
  if (channel<0 || channel>=WPI_MaxSPIChannels) {
    fprintf (stderr, "wiringPiSPI: Invalid SPI channel (%d, valid range 0-%d)", channel, WPI_MaxSPIChannels-1);
    return -EINVAL;
  }
  if (number<0 || number>=WPI_MaxSPINumbers) {
    fprintf (stderr, "wiringPiSPI: Invalid SPI number  (%d, valid range 0-%d)", number, WPI_MaxSPINumbers-1);
    return -EINVAL;
  }

  return 0;  //sucess
}


#define RETURN_ON_LIMIT_FAIL int ret = SPICheckLimits(number, channel); if(ret!=0) { return ret; };

/*
 * wiringPiSPIGetFd:
 *	Return the file-descriptor for the given channel
 *********************************************************************************
 */

int wiringPiSPIxGetFd(const int number, int channel)
{
  if (SPICheckLimits(number, channel)!=0) {
    return -1;
  }
  return spiFds[number][channel];
}

int wiringPiSPIGetFd(int channel) {
  return wiringPiSPIxGetFd(0, channel);
}


/*
 * wiringPiSPIDataRW:
 *	Write and Read a block of data over the SPI bus.
 *	Note the data ia being read into the transmit buffer, so will
 *	overwrite it!
 *	This is also a full-duplex operation.
 *********************************************************************************
 */

int wiringPiSPIxDataRW (const int number, const int channel, unsigned char *data, const int len)
{

  RETURN_ON_LIMIT_FAIL
  if (-1==spiFds[number][channel]) {
    fprintf (stderr, "wiringPiSPI: Invalid SPI number/channel (need wiringPiSPIxSetupMode before read/write)");
    return -EBADF;
  }

  struct spi_ioc_transfer spi ;
// Mentioned in spidev.h but not used in the original kernel documentation
//	test program )-:
  memset (&spi, 0, sizeof (spi)) ;

  spi.tx_buf        = (unsigned long)data ;
  spi.rx_buf        = (unsigned long)data ;
  spi.len           = len ;
  spi.delay_usecs   = spiDelay ;
  spi.speed_hz      = spiSpeeds [number][channel] ;
  spi.bits_per_word = spiBPW ;

  return ioctl (spiFds[number][channel], SPI_IOC_MESSAGE(1), &spi) ;
}

int wiringPiSPIDataRW (int channel, unsigned char *data, int len) {
  return wiringPiSPIxDataRW(0, channel, data, len);
}

/*
 * wiringPiSPISetupMode:
 *	Open the SPI device, and set it up, with the mode, etc.
 *********************************************************************************
 */


int wiringPiSPIxSetupMode(const int number, const int channel, const int speed, const int mode)
{
  int fd ;
  char spiDev [32] ;

  RETURN_ON_LIMIT_FAIL
  if (mode<0 || mode>3) { // Mode is 0, 1, 2 or 3 original
    fprintf (stderr, "wiringPiSPI: Invalid mode (%d, valid range 0-%d)", mode, 3);
    return -EINVAL;
  }

  snprintf (spiDev, 31, "/dev/spidev%d.%d", number, channel) ;
  if ((fd = open (spiDev, O_RDWR)) < 0) {
    return wiringPiFailure (WPI_ALMOST, "Unable to open SPI device %s: %s\n", spiDev, strerror (errno)) ;
  }
  spiSpeeds [number][channel] = speed ;
  spiFds    [number][channel] = fd ;

// Set SPI parameters.

  if (ioctl (fd, SPI_IOC_WR_MODE, &mode)            < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI mode change failure: %s\n", strerror (errno)) ;
  
  if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &spiBPW) < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI BPW change failure: %s\n", strerror (errno)) ;

  if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)   < 0)
    return wiringPiFailure (WPI_ALMOST, "SPI speed change failure: %s\n", strerror (errno)) ;

  return fd ;
}


int wiringPiSPISetupMode (int channel, int speed, int mode) {
 return wiringPiSPIxSetupMode (0, channel, speed, mode);
}


/*
 * wiringPiSPISetup:
 *	Open the SPI device, and set it up, etc. in the default MODE 0
 *********************************************************************************
 */

int wiringPiSPISetup (int channel, int speed) {
  return wiringPiSPIxSetupMode(0, channel, speed, 0) ;
}


int wiringPiSPIxClose (const int number, const int channel) {

  RETURN_ON_LIMIT_FAIL
  if (spiFds[number][channel]>0) {
    ret = close(spiFds[number][channel]);
  }
  spiSpeeds [number][channel] = 0 ;
  spiFds    [number][channel] = -1 ;
  return ret;
}

int wiringPiSPIClose (const int channel) {
  return wiringPiSPIxClose (0, channel);
}

