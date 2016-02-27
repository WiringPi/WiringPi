/*
 * mcp3422.c:
 *	Extend wiringPi with the MCP3422 I2C ADC chip
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

#define	MCP3422_SR_3_75	0
#define	MCP3422_SR_15	1
#define	MCP3422_SR_60	2
#define	MCP3422_SR_240	3

#define	MCP3422_GAIN_1	0
#define	MCP3422_GAIN_2	1
#define	MCP3422_GAIN_4	2
#define	MCP3422_GAIN_8	3


#ifdef __cplusplus
extern "C" {
#endif

extern int mcp3422Setup (int pinBase, int i2cAddress, int sampleRate, int gain) ;

#ifdef __cplusplus
}
#endif
