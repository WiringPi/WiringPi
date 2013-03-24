/*
 * wiringPiI2C.c:
 *	Simplified I2C access routines
 *	Copyright (c) 2013 Gordon Henderson
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
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "wiringPi.h"
#include "wiringPiI2C.h"


/*
 * wiringPiI2CRead:
 *	Simple device read
 *********************************************************************************
 */

int wiringPiI2CRead (int fd)
{
  return i2c_smbus_read_byte (fd) ;
}


/*
 * wiringPiI2CReadReg8: wiringPiI2CReadReg16:
 *	Read an 8 or 16-bit value from a regsiter on the device
 *********************************************************************************
 */

int wiringPiI2CReadReg8 (int fd, int reg)
{
  return i2c_smbus_read_byte_data (fd, reg) ;
}

int wiringPiI2CReadReg16 (int fd, int reg)
{
  return i2c_smbus_read_word_data (fd, reg) ;
}


/*
 * wiringPiI2CWrite:
 *	Simple device write
 *********************************************************************************
 */

int wiringPiI2CWrite (int fd, int data)
{
  return i2c_smbus_write_byte (fd, data) ;
}


/*
 * wiringPiI2CWriteReg8: wiringPiI2CWriteReg16:
 *	Write an 8 or 16-bit value to the given register
 *********************************************************************************
 */

int wiringPiI2CWriteReg8 (int fd, int reg, int data)
{
  return i2c_smbus_write_byte_data (fd, reg, data) ;
}

int wiringPiI2CWriteReg16 (int fd, int reg, int data)
{
  return i2c_smbus_write_word_data (fd, reg, data) ;
}


/*
 * wiringPiI2CSetup:
 *	Open the I2C device, and regsiter the target device
 *********************************************************************************
 */

int wiringPiI2CSetup (int devId)
{
  int rev, fd ;
  char *device ;

  if ((rev = piBoardRev ()) < 0)
  {
    fprintf (stderr, "wiringPiI2CSetup: Unable to determine Pi board revision\n") ;
    exit (1) ;
  }

  if (rev == 1)
    device = "/dev/i2c-0" ;
  else
    device = "/dev/i2c-1" ;

  if ((fd = open (device, O_RDWR)) < 0)
    return -1 ;

  if (ioctl (fd, I2C_SLAVE, devId) < 0)
    return -1 ;

  return fd ;
}
