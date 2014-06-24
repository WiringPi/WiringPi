/*
 * pins.c:
 *	Just display a handy Pi pinnout diagram.
 *	Copyright (c) 2012-2013 Gordon Henderson
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
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>

extern int wpMode ;

void doPins (void)
{
  int model, rev, mem ;
  char *maker ;

  piBoardId (&model, &rev, &mem, &maker) ;
  if (model == PI_MODEL_CM)
  {
    printf ("This Raspberry Pi is a Compute Module.\n") ;
    printf ("    (who knows what's been done to the pins!)\n") ;
    return ;
  }

  printf ("This Raspberry Pi is a revision %d board.\n", piBoardRev ()) ;

  printf (
" +-----+--------+------------+--------+-----+\n"
" | Pin |   Name ||  P1 Pin  ||  Name  | Pin |\n"
" +-----+--------+------------+--------+-----+\n"
" |     |  3.3v  ||  1 oo 2  ||     5v |     |\n"
" |   8 |   SDA  ||  3 oo 4  ||     5v |     |\n"
" |   9 |    SCL ||  5 oo 6  ||    Gnd |     |\n"
" |   7 | GPIO 7 ||  7 oo 8  ||    TxD |  15 |\n"
" |     |    GND ||  9 oo 10 ||    RxD |  16 |\n"
" |   0 | GPIO 0 || 11 oo 12 || GPIO 1 |   1 |\n"
" |   2 | GPIO 2 || 13 oo 14 ||    Gnd |     |\n"
" |   3 | GPIO 3 || 15 oo 16 || GPIO 4 |   4 |\n"
" |     |   3.3v || 17 oo 18 || GPIO 5 |   5 |\n"
" |  12 |   MOSI || 19 oo 20 ||    Gnd |     |\n"
" |  13 |   MISO || 21 oo 22 || GPIO 6 |   6 |\n"
" |  14 |   SCLK || 23 oo 24 ||   CE 0 |  10 |\n"
" |     |    Gnd || 25 oo 26 ||   CE 1 |  11 |\n"
" +-----+--------+------------+--------+-----+\n") ;

/***
 +---
 |  5v|  5v| Gnd | TxD | RxD | G1  | Gnd |  G4  |  G5  | G
 |  2 |  4 |  6 |  8 | 10 | 12 | 14 | 16 | 18 | 20 | 22 | 24 | 26 |\n"
 |  1 |  3 |  5 |  7 |  9 | 11 | 13 | 15 | 17 | 19 | 21 | 23 | 25 |\n"
***/

}
