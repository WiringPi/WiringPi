/*
 * drcNet.h:
 *	Extend wiringPi with the DRC Network protocol (e.g. to another Pi)
 *	Copyright (c) 2016-2017 Gordon Henderson
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

/*********
struct drcNetStruct
{
  uint32_t pin ;
  uint32_t cmd ;
  uint32_t data ;
} ;
**************/

#ifdef __cplusplus
extern "C" {
#endif

extern int drcSetupNet (const int pinBase, const int numPins, const char *ipAddress, const char *port, const char *password) ;

#ifdef __cplusplus
}
#endif
