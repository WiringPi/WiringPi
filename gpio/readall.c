/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2018 Gordon Henderson
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

#ifndef TRUE
#  define       TRUE    (1==1)
#  define       FALSE   (1==2)
#endif

/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 *********************************************************************************
 */

static void doReadallExternal (void)
{
  int pin ;

  printf ("+------+---------+--------+\n") ;
  printf ("|  Pin | Digital | Analog |\n") ;
  printf ("+------+---------+--------+\n") ;

  for (pin = wiringPiNodes->pinBase ; pin <= wiringPiNodes->pinMax ; ++pin)
    printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin)) ;

  printf ("+------+---------+--------+\n") ;
}


/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 *********************************************************************************
 */

static char *alts [] =
{
  "IN", "OUT", "ALT0", "ALT1", "ALT2", "ALT3", "ALT4", "ALT5"
} ;

static const char *pupd [] =
{
	"DSBLD", "P/U", "P/D"
};

static int physToWpi [64] = 
{
  -1,           // 0
  -1, -1,       // 1, 2
   8, 15,
   9, 29,
   7, -1,
  -1, 16,
   0,  1,
  -1, -1,
   3, -1,
  -1,  5,
  12,  4,
  13,  6,
  14, 10,
   2, 11,       // 25, 26
  -1, -1,	// Actually I2C, but not used
  21, 30,
  22, 26,
  23, 31,
  24, 27,
  25, 28,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  17, 18,
  19, 20,
  -1, -1, -1, -1, -1, -1, -1, -1, -1
} ;

/*----------------------------------------------------------------------------*/
static int physToWpi_Edge [64] =
{
	-1,	// 0
	-1, -1,	// 1, 2
    -1, -1, //	-1, 13,
    -1, -1, //	-1, 12,
	-1, -1,
	-1,  9,
	-1,  8,
	-1, -1,
	-1, -1,
	-1,  0,
	-1, 11,
	-1,  7,
	-1,  2,
	17,  3,	
	-1, -1,	
    -1,  6,  //	10,  6,
    -1,  5,  //	14,  5,
	-1,  4,
	-1,  1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1
} ;

/*------------------------------------------------------------------------------------------*/
static const char *physNamesKhadasVim1[64] = {
	NULL,
	"      5V","GND     ",
	"      5V","PIN.DV25",
	" HUB_DM1","PIN.DV24",
	" HUB_DP1","GND     ",
	"     GND","PIN.DV27",
	"      5V","PIN.DV26",
	" HUB_DM2","3.3V    ",
	" HUB_DP2","GND     ",
	"     GND","PIN.H7  ",
	" ADC.CH0","PIN.H6  ",
	"     GND","PIN.H9  ",
	" ADC.CH2","PIN.H8  ",
	"   SPDIF","PIN.AO6 ",
	"     GND","GND     ",
	" PIN.AO5","PIN.AO3 ",
	" PIN.AO4","RTC_CLK ",
	"     GND","PIN.H5  ",
	" PIN.AO1","PWR_EN  ",
	" PIN.AO2","PWM_F   ",
	"    3.3V","GND     ",
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

static const char *physNamesKhadasVim2[64] = {
	NULL,
	"      5V","GND     ",
	"      5v","PIN.DV25",
	"  USB_DM","PIN.DV24",
	"  USB_DP","GND     ",
	"     GND","PIN.DV27",
	"PIN.DV21","PIN.DV26",
	"PIN.DV22","3.3V    ",
	"PIN.DV23","GND     ",
	"     GND","PIN.H7  ",
	"    ADC0","PIN.H6  ",
	"    1.8V","PIN.H9  ",
	"    ADC1","PIN.H8  ",
	"   SPDIF","PIN.AO6 ",
	"     GND","GND     ",
	" PIN.AO5","PIN.DV29",
	" PIN.AO4","RTC_CLK ",
	"     GND","PIN.H5  ",
	" PIN.AO1","EXP_INT ",
	" PIN.AO0","PIN.DV13",
	"    3.3v","GND     ",
	//Not used
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

static const char *physNamesKhadasVim3[64] = {
	NULL,
	"      5V","GND     ",
	"      5V","PIN.A15 ",
	"  USB_DM","PIN.A14 ",
	"  USB_DP","GND     ",
	"     GND","PIN.AO2 ",
	"  MCU3V3","PIN.AO3 ",
	" MCUNRST","3V3     ",
	" MCUSWIM","GND     ",
	"     GND","PIN.A1  ",
	"    ADC0","PIN.A0  ",
	"     1V8","PIN.A3  ",
	"    ADC1","PIN.A2  ",
	"   SPDIF","PIN.A4  ",
	"    GND3","GND     ",
	"  PIN.H6","PWM-F   ",
	"  PIN.H7","RTC     ",
	"     GND","PIN.H4  ",
	" PIN.AO1","MCU-FA1 ",
	" PIN.AO0","PIN.Z15 ",
	"     3V3","GND     ",
	//Not used
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
};

/*----------------------------------------------------------------------------*/
static const char *physNamesKhadasEdge [64] =
{
	NULL,

	"               5V", "GND(0V)                   ",
	"               5V", "SPI3_RXD/I2C0_SCK/GPIO1_C0",
	"         HOST1_DM", "SPI3_TXD/I2C0_SDA/GPIO1_B7",
	"         HOST1_DP", "GND(0V)                   ",
	"          GND(0V)", "I2C2_SCL/GPIO2_A1         ",
	"           MCU_TX", "I2C2_SDA/GPIO2_A0         ",
	"         MCU_NRST", "3.3V                      ",
	"         MCU_SWIM", "GND(0V)                   ",
	"          GND(0V)", "I2S0_SCLK/GPIO3_D0        ",
	"          ADC_IN2", "I2S_CLK/GPIO4_A0          ",
	"             1.8V", "I2S0_SDO0/GPIO3_D7        ",
	"          ADC_IN3", "2S0_LRCK_TX/GPIO3_D2      ",
	"   SPDIF/GPIO3_C0", "I2S0_SDI0/GPIO3_D3        ",
	"           GND(0V", "GND(0V)                   ",
	" SPI3_CS/GPIO1_C2", "I2S0_SDI3SDO1/GPIO3_D6    ",
	"SPI3_CLK/GPIO1_C1", "2S0_SDI2SDO2/GPIO3_D5     ",
	"          GND(0V)", "I2S0_SDI1SDO3/GPIO3_D4    ",
	"          UART_RX", "I2S0_LRCK_RX/GPIO3_D1     ",
	"          UART_TX", "MCU_PA1                   ",
	"             3.3V", "GND(0V)                   ",	

	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,
} ;

static void readallPhysKhadas (int model, int UNU rev, int physPin, const char *physNames[])
{
	int pin;

	//GPIO, wPi pin number
	if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
		printf (" |      |    ") ;
	else if (physPinToGpio (physPin) != -1)
		printf (" |  %3d | %3d", physPinToGpio (physPin), physToWpi [physPin]);
	else
		printf (" |      | %3d", physToWpi [physPin]);

	// GPIO pin name
	printf (" | %s", physNames [physPin]);

	// GPIO pin mode, value, drive strength, pupd
	if ((physToWpi [physPin] == -1) || (physPinToGpio (physPin) == -1)){
		printf (" |      |   |    |      ") ;
	}

	else {
		if (wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if(wpMode == MODE_PHYS)
			pin = physPin;
		else{
			pin = physToWpi [physPin];
		}
		printf (" | %4s", alts [getAlt (pin)]);
		printf (" | %d", digitalRead (pin));	
		switch(model) {
			case MODEL_KHADAS_VIM1:
			case MODEL_KHADAS_VIM2:
			case MODEL_KHADAS_VIM3:
				printf (" |    | %5s", pupd[getPUPD(pin)]);
				break;
			default:
				break;
		}
	}

	// Physical pin number
	printf (" | %2d", (physPin+1)/2);
	printf (" || %-2d", (physPin+41)/2);
	++physPin;

	// GPIO pin mode, value, drive strength, pupd
	if ((physToWpi [physPin] == -1) || (physPinToGpio (physPin) == -1))
		printf (" |       |    |   |     ");
	else {
		if(wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if(wpMode == MODE_PHYS)
			pin = physPin;
		else
			pin = physToWpi [physPin];

		switch(model){
			case MODEL_KHADAS_VIM1:
			case MODEL_KHADAS_VIM2:
			case MODEL_KHADAS_VIM3:
				printf (" | %-5s |   ", pupd[getPUPD(pin)]);
				break;
			default:
				break;
		}
		printf (" | %d", digitalRead (pin));
		printf (" | %-4s", alts [getAlt (pin)]);
	}

	// GPIO pin name
	printf (" | %-6s", physNames [physPin]);

	//GPIO, wPi pin number
	if ((physPinToGpio (physPin) == -1) && (physToWpi [physPin] == -1))
		printf (" |     |     ") ;
	else if(physPinToGpio (physPin) != -1)
		printf (" | %-3d |  %-3d", physToWpi [physPin], physPinToGpio (physPin));
	else
		printf (" | %-3d |     ", physToWpi [physPin]);
	
	printf (" |\n") ;
}

static void readallPhysKhadas_edge (int model, int UNU rev, int physPin, const char *physNames[])
{
	int pin;

	//GPIO, wPi pin number
	if ((physPinToGpio (physPin) == -1) && (physToWpi_Edge [physPin] == -1))
		printf (" |      |    ") ;
	else if (physPinToGpio (physPin) != -1)
		printf (" |  %3d | %3d", physPinToGpio (physPin), physToWpi_Edge [physPin]);
	else
		printf (" |      | %3d", physToWpi_Edge [physPin]);

	// GPIO pin name
	printf (" | %s", physNames [physPin]);

	// GPIO pin mode, value, drive strength, pupd
	if ((physToWpi_Edge [physPin] == -1) || (physPinToGpio (physPin) == -1)){
		printf (" |      |   |    |      ") ;
	}

	else {
		if (wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if(wpMode == MODE_PHYS)
			pin = physPin;
		else{
			pin = physToWpi_Edge [physPin];
		}
		printf (" | %4s", alts [getAlt (pin)]);
		printf (" | %d", digitalRead (pin));	
		switch(model) {
			case MODEL_KHADAS_VIM1:
			case MODEL_KHADAS_VIM2:
			case MODEL_KHADAS_VIM3:
				printf (" |    | %5s", pupd[getPUPD(pin)]);
				break;
			case MODEL_KHADAS_EDGE:
				printf (" |    |      ");
				break;
			default:
				break;
		}
	}

	// Physical pin number
	printf (" | %2d", (physPin+1)/2);
	printf (" || %-2d", (physPin+41)/2);
	++physPin;

	// GPIO pin mode, value, drive strength, pupd
	if ((physToWpi_Edge [physPin] == -1) || (physPinToGpio (physPin) == -1))
		printf (" |       |    |   |     ");
	else {
		if(wpMode == MODE_GPIO)
			pin = physPinToGpio (physPin);
		else if(wpMode == MODE_PHYS)
			pin = physPin;
		else
			pin = physToWpi_Edge [physPin];

		switch(model){
			case MODEL_KHADAS_VIM1:
			case MODEL_KHADAS_VIM2:
			case MODEL_KHADAS_VIM3:
				printf (" | %-5s |   ", pupd[getPUPD(pin)]);
				break;
			case MODEL_KHADAS_EDGE:
				printf (" |       |   ");
				break;				
			default:
				break;
		}
		printf (" | %d", digitalRead (pin));
		printf (" | %-4s", alts [getAlt (pin)]);
	}

	// GPIO pin name
	printf (" | %-6s", physNames [physPin]);

	//GPIO, wPi pin number
	if ((physPinToGpio (physPin) == -1) && (physToWpi_Edge [physPin] == -1))
		printf (" |     |     ") ;
	else if(physPinToGpio (physPin) != -1)
		printf (" | %-3d |  %-3d", physToWpi_Edge [physPin], physPinToGpio (physPin));
	else
		printf (" | %-3d |     ", physToWpi_Edge [physPin]);
	
	printf (" |\n") ;
}

/*--------------------------------------------------------------------------------------*/
void ReadallKhadas(int model, int rev, const char *physNames[])
{
	int pin;
	if(MODEL_KHADAS_EDGE == model){
		printf (" | GPIO | wPi |        Name       | Mode | V | DS | PU/PD | Physical | PU/PD | DS | V | Mode |            Name            | wPi | GPIO |\n") ;
		printf (" +------+-----+-------------------+------+---+----+-------+----++----+-------+----+---+------+----------------------------+-----+------+\n") ;
		for (pin = 1 ; pin <= 40 ; pin += 2)
			readallPhysKhadas_edge (model, rev, pin, physNames) ;
		printf (" +------+-----+-------------------+------+---+----+-------+----++----+-------+----+---+------+----------------------------+-----+------+\n") ;
	}
	else{
		printf (" | GPIO | wPi |   Name   | Mode | V | DS | PU/PD | Physical | PU/PD | DS | V | Mode |   Name   | wPi | GPIO |\n");
		printf (" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n");
		for (pin = 1 ; pin <= 40 ; pin += 2)
			readallPhysKhadas (model, rev, pin, physNames);
		printf (" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n");
	}
}

/*--------------------------------------------------------------------------------------*/
/*									doReadall											*/
/*	Read all the GPIO pins																*/
/*	We also want to use this to read the state of pins on an externally					*/
/*	connected device, so we need to do some fiddling with the internal					*/
/*	wiringPi node structures - since the gpio command can only use						*/
/*	one external device at a time, we'll use that to our advantage...					*/
/*--------------------------------------------------------------------------------------*/
void doReadall(void)
{
	int model, rev, mem, maker, overVolted;
	
	const char **physNames;

	// External readall
	if (wiringPiNodes != NULL) {
		doReadallExternal ();
		return;
	}

	piBoardId(&model, &rev, &mem, &maker, &overVolted);

	switch(model){
		case MODEL_KHADAS_VIM1:
			printf (" +------+-----+----------+------+---+----+---- Model  KDADAS-VIM1---+----+---+------+----------+-----+------+\n") ;
			physNames = physNamesKhadasVim1;
			break;
		case MODEL_KHADAS_VIM2:
			printf (" +------+-----+----------+------+---+----+---- Model  KDADAS-VIM2---+----+---+------+----------+-----+------+\n") ;
			physNames = physNamesKhadasVim2;
			break;
		case MODEL_KHADAS_VIM3:
			printf (" +------+-----+----------+------+---+----+---- Model  KDADAS-VIM3---+----+---+------+----------+-----+------+\n") ;
			physNames = physNamesKhadasVim3;
			break;
		case MODEL_KHADAS_EDGE:
			printf (" +------+-----+-------------------+------+---+----+--- Model  KHADAS-EDGE ---+----+---+------+----------------------------+-----+------+\n");
			physNames = physNamesKhadasEdge;
			break;
		default:
			printf ("Oops - unable to determine board type... model: %d\n", model);
			return;
	}
	ReadallKhadas(model, rev, physNames);
}

/*--------------------------------------------------------------------------------------*/
/*								doAllReadall											*/
/*	Force reading of all pins regardless of Pi model									*/
/*--------------------------------------------------------------------------------------*/
void doAllReadall(void)
{
	doReadall();
}

/*--------------------------------------------------------------------------------------*/
void doQmode(int argc, char *argv[])
{
	int pin;
	if(argc != 3){
		fprintf (stderr, "Usage: %s qmode pin\n", argv [0]) ;
		exit(EXIT_FAILURE);
	}

	pin = atoi(argv[2]);
	printf("%s\n", alts[getAlt(pin)]);
}
/*--------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------*/
