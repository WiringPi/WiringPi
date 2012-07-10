/*
 * serial.h:
 *	Handle a serial port
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifndef	_STDINT_H
#  include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int   serialOpen      (char *device, int baud) ;
extern void  serialClose     (int fd) ;
extern void  serialPutchar   (int fd, uint8_t c) ;
extern void  serialPuts      (int fd, char *s) ;
extern void  serialPrintf    (int fd, char *message, ...) ;
extern int   serialDataAvail (int fd) ;
extern int   serialGetchar   (int fd) ;

#ifdef __cplusplus
}
#endif
