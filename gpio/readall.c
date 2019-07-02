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
   8, -1,
   9, -1,
   7, 15,
  -1, 16,
   0,  1,
   2, -1,
   3,  4,
  -1,  5,
  12, -1,
  13,  6,
  14, 10,
  -1, 11,       // 25, 26
  30, 31,	// Actually I2C, but not used
  21, -1,
  22, 26,
  23, -1,
  24, 27,
  25, 28,
  -1, 29,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  17, 18,
  19, 20,
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


static void readallPhysKhadas (int model, int rev, int physPin, const char *physNames[])
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

/*--------------------------------------------------------------------------------------*/
void ReadallKhadas(int model, int rev, const char *physNames[])
{
	int pin;

	printf (" | GPIO | wPi |   Name   | Mode | V | DS | PU/PD | Physical | PU/PD | DS | V | Mode |   Name   | wPi | GPIO |\n");
	printf (" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n");
	for (pin = 1 ; pin <= 40 ; pin += 2)
		readallPhysKhadas (model, rev, pin, physNames);
	printf (" +------+-----+----------+------+---+----+-------+----++----+-------+----+---+------+----------+-----+------+\n");
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
