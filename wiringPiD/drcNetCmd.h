/*
 * drcNetCmd.c:
 *	Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/WiringPi/WiringPi/
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

#define	DEFAULT_SERVER_PORT	6124

#define	DRCN_PIN_MODE		1
#define	DRCN_PULL_UP_DN		2

#define	DRCN_DIGITAL_WRITE	3
#define	DRCN_DIGITAL_WRITE8	4
#define	DRCN_ANALOG_WRITE	5
#define	DRCN_PWM_WRITE		6

#define	DRCN_DIGITAL_READ	7
#define	DRCN_DIGITAL_READ8	8
#define	DRCN_ANALOG_READ	9


struct drcNetComStruct
{
  uint32_t pin ;
  uint32_t cmd ;
  uint32_t data ;
};
