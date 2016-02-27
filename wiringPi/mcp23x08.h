/*
 * mcp23x17:
 *	Copyright (c) 2012-2013 Gordon Henderson
 *
 *	Header file for code using the MCP23x17 GPIO expander chip.
 *	This comes in 2 flavours: MCP23017 which has an I2C interface,
 *	an the MXP23S17 which has an SPI interface.
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


// MCP23x17 Registers

#define	IODIRA		0x00
#define	IPOLA		0x02
#define	GPINTENA	0x04
#define	DEFVALA		0x06
#define	INTCONA		0x08
#define	IOCON		0x0A
#define	GPPUA		0x0C
#define	INTFA		0x0E
#define	INTCAPA		0x10
#define	GPIOA		0x12
#define	OLATA		0x14

#define	IODIRB		0x01
#define	IPOLB		0x03
#define	GPINTENB	0x05
#define	DEFVALB		0x07
#define	INTCONB		0x09
#define	IOCONB		0x0B
#define	GPPUB		0x0D
#define	INTFB		0x0F
#define	INTCAPB		0x11
#define	GPIOB		0x13
#define	OLATB		0x15

// Bits in the IOCON register

#define	IOCON_UNUSED	0x01
#define	IOCON_INTPOL	0x02
#define	IOCON_ODR	0x04
#define	IOCON_HAEN	0x08
#define	IOCON_DISSLW	0x10
#define	IOCON_SEQOP	0x20
#define	IOCON_MIRROR	0x40
#define	IOCON_BANK_MODE	0x80

// Default initialisation mode

#define	IOCON_INIT	(IOCON_SEQOP)

// SPI Command codes

#define	CMD_WRITE	0x40
#define CMD_READ	0x41
