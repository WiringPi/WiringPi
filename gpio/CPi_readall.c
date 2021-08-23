#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "CPi.h"

#ifdef CONFIG_CLOCKWORKPI_A04

int bcmToGpioCPi[64] =
{
	106,  107,      // 0, 1
	104, 10,      // 2, 3
	3, 9,      // 4  5
	4,  90,      // 6, 7
	92,  158,      // 8, 9
	156,  105,      //10,11
	146,  150,      //12,13
	81,  80,      //14,15

	82, 83,      //16,17
	131,  132,      //18,19
	134,  135,      //20,21
	89,  88,      //22,23
	84,  85,      //24,25
	86, 87,      //26,27
	112,  113,    //28,29
	109,   157,    //30,31

	148,  147,      //32,33
	100,  101,      //34,35
	102, 103,      //36,37
	97,  98,      //38,39
	99,  96,      //40,41
	110, 111,      //42,43
	64,  65,      //44,45
	-1, -1,      //46,47

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,// ... 63	
}

#endif

#ifdef CONFIG_CLOCKWORKPI_A06

int bcmToGpioCPi[64] =
{
	106,  107,      // 0, 1
	104, 10,      // 2, 3
	3, 9,      // 4  5
	4,  90,      // 6, 7
	92,  158,      // 8, 9
	156,  105,      //10,11
	146,  150,      //12,13
	81,  80,      //14,15

	82, 83,      //16,17
	131,  132,      //18,19
	134,  135,      //20,21
	89,  88,      //22,23
	84,  85,      //24,25
	86, 87,      //26,27
	112,  113,    //28,29
	109,   157,    //30,31

	148,  147,      //32,33
	100,  101,      //34,35
	102, 103,      //36,37
	97,  98,      //38,39
	99,  96,      //40,41
	110, 111,      //42,43
	64,  65,      //44,45
	-1, -1,      //46,47

	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,// ... 63	
};

#endif

static char *alts [] =
{
  "IN", "OUT", "ALT2", "ALT3", "ALT4", "ALT5", "ALT6", "OFF"
} ;

void CPiReadAll(void)
{
	int pin, pin2;
	int tmp = wiringPiDebug;
	wiringPiDebug = FALSE;

	printf (" +-----+------+------+---+-----+------+------+---+\n");
	printf (" | BCM | GPIO | Mode | V | BCM | GPIO | Mode | V |\n");
	printf (" +-----+------+------+---+-----+------+------+---+\n");

	for (pin = 0 ; pin < 23; pin ++) {
		printf (" | %3d", pin);
		printf (" | %4d", bcmToGpioCPi[pin]);
		printf (" | %4s", alts [CPi_get_gpio_mode(bcmToGpioCPi[pin])]);
		printf (" | %d", CPi_digitalRead(bcmToGpioCPi[pin])) ;
		pin2 = pin + 23;
		printf (" | %3d", pin2);
		printf (" | %4d", bcmToGpioCPi[pin2]);
		printf (" | %4s", alts [CPi_get_gpio_mode(bcmToGpioCPi[pin2])]);
		printf (" | %d", CPi_digitalRead(bcmToGpioCPi[pin2])) ;
		printf (" |\n") ;
	}

	printf (" +-----+------+------+---+-----+------+------+---+\n");
	printf (" | BCM | GPIO | Mode | V | BCM | GPIO | Mode | V |\n");
	printf (" +-----+------+------+---+-----+------+------+---+\n");

	wiringPiDebug = tmp;
}

void CPiReadAllRaw(void)
{
	int pin, pin2, i;
	int tmp = wiringPiDebug;
	wiringPiDebug = FALSE;

	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");
	printf (" | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V |\n");
	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");

	for (pin = 0 ; pin < 32; pin++) {
		pin2 = pin;
		for(i = 0; i < 5; i++) {
			printf (" | %4d", pin2) ;
			printf (" | %4s", alts [CPi_get_gpio_mode(pin2)]) ;
			printf (" | %d", CPi_digitalRead(pin2)) ;
			pin2 += 32;
		}
		printf (" |\n") ;
	}

	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");
	printf (" | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V | GPIO | Mode | V |\n");
	printf (" +------+------+---+------+------+---+------+------+---+------+------+---+------+------+---+\n");

	wiringPiDebug = tmp;
}

