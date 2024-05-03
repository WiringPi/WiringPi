/*
 * pseudoPins.c:
 *	Extend wiringPi with a number of pseudo pins which can be
 *	digitally or analog written/read.
 *
 *	Note:
 *		Just one set of pseudo pins can exist per Raspberry Pi.
 *		These pins are shared between all programs running on
 *		that Raspberry Pi. The values are also persistant as
 *		they live in shared RAM. This gives you a means for
 *		temporary variable storing/sharing between programs,
 *		or for other cunning things I've not thought of yet..
 *
 *	Copyright (c) 2012-2016 Gordon Henderson
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

#define	SHARED_NAME	"wiringPiPseudoPins"
#define	PSEUDO_PINS	64

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <wiringPi.h>

#include "pseudoPins.h"

static int myAnalogRead(struct wiringPiNodeStruct *node, int pin)
{
  int *ptr = (int *)(intptr_t)node->data0; // Cast to intptr_t to handle pointer-to-integer conversion
  int myPin = pin - node->pinBase;

  return *(ptr + myPin);
}

static void myAnalogWrite(struct wiringPiNodeStruct *node, int pin, int value)
{
  int *ptr = (int *)(intptr_t)node->data0;
  int myPin = pin - node->pinBase;

  *(ptr + myPin) = value;
}


/*
 * pseudoPinsSetup:
 *	Create a new wiringPi device node for the pseudoPins driver
 *********************************************************************************
 */


int pseudoPinsSetup(const int pinBase)
{
    struct wiringPiNodeStruct *node;
    void *ptr;

    node = wiringPiNewNode(pinBase, PSEUDO_PINS);
    if (node == NULL) {
      fprintf(stderr, "Error creating new wiringPi node");
      return FALSE;
    }

    node->fd = shm_open(SHARED_NAME, O_CREAT | O_RDWR, 0666);
    if (node->fd < 0) {
      perror("Error opening shared memory");
      return FALSE;
    }

    if (ftruncate(node->fd, PSEUDO_PINS * sizeof(int)) < 0) {
      perror("Error resizing shared memory");
      return FALSE;
    }

    ptr = mmap(NULL, PSEUDO_PINS * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, node->fd, 0);
    if (ptr == MAP_FAILED) {
      perror("Error mapping shared memory");
      return FALSE;
    }

    node->data0 = (unsigned int)(uintptr_t)ptr;

    node->analogRead = myAnalogRead;
    node->analogWrite = myAnalogWrite;

    return TRUE;
}
