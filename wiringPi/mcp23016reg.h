/*
 * mcp23016:
 *	Copyright (c) 2012-2013 Gordon Henderson
 *
 *	Header file for code using the MCP23016 GPIO expander
 *	chip.
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

// MCP23016 Registers

#define	MCP23016_GP0		0x00
#define	MCP23016_GP1		0x01
#define	MCP23016_OLAT0		0x02
#define	MCP23016_OLAT1		0x03
#define	MCP23016_IPOL0		0x04
#define	MCP23016_IPOL1		0x05
#define	MCP23016_IODIR0		0x06
#define	MCP23016_IODIR1		0x07
#define	MCP23016_INTCAP0	0x08
#define	MCP23016_INTCAP1	0x09
#define	MCP23016_IOCON0		0x0A
#define	MCP23016_IOCON1		0x0B

// Bits in the IOCON register

#define	IOCON_IARES	0x01

// Default initialisation mode

#define	IOCON_INIT	0
