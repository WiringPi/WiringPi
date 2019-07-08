/************************************************************************************/
/*																					*/
/* 				WiringPi KHADAS VIM1 Board control file								*/
/*																					*/
/************************************************************************************/

/*----------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <sys/mman.h>
#include <sys/utsname.h>

#include "softPwm.h"
#include "softTone.h"
#include "wiringPi.h"
#include "khadas_vim1.h"
/*----------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------
	wiringPi gpio map define
  ----------------------------------------------------------------------------------*/

  static const int pinToGpio_rev[64] = {
  	//wiringPi number to native gpio number
		-1,175,		//	0 |  1 :					 | GPIODV_26
		-1,	-1,		//	2 |  3 :
	   122,123,		//	4 |  5 : 					 | GPIOH_7 
	   125,	-1,		//	6 |  7 :			 GPIOH_9 |
		-1,	-1,		//	8 |  9 :
	   124,136,		// 10 | 11 :			 GPIOH_8 | GPIOAO_6
		-1,	-1,		// 12 | 13 :
		-1,174,		// 14 | 15 :
	   176,	-1,		// 16 | 17 :      	   GPIODV_27 |
		-1,	-1,		// 18 | 19 :
		-1,135,		// 20 | 21 :					 | GPIOAO_5
	   134,	-1,		// 22 | 23 :	  		GPIOAO_4 |
	   121,132,		// 24 | 25 :	   		GPIOAO_1 | GPIOAO_2
		-1,	-1,		// 26 | 27 :		
		-1,173,		// 28 | 29 :
		-1,121,		// 30 | 31 :
		// Padding:
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	-1,	-1, //32to47
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //48to63
  };


  static const int phyToGpio_rev[64] = {
  	//physical header pin number to native gpio number
	-1,			//  0
	-1,	-1,		//  1 | 21 :   			   	     5v | GND
	-1,174,		//  2 | 22 : 		  		     5V | I2C_SCK_A(GPIODV_25)
	-1,173,		//  3 | 23 :   	 		    HUB_DM1 | I2C_SDA_A(GPIODV_24)
	-1,	-1,		//  4 | 24 :   	  		    HUB_DP1 | GND
	-1,176,		//  5 | 25 :	  	  		    GND | I2C_SCK_B(GPIODV_27)
	-1,175,		// 	6 | 26 :     	  		     5V | I2C_SDA_B(GPIODV_26)
	-1,	-1,		// 	7 | 27 :  			    HUB_DM2 | 3.3V
	-1,	-1,		// 	8 | 28 :    		    HUB_DP2 | GND
	-1,123,		// 	9 | 29 :     		   	    GND | GPIOH_7
	-1,122,		// 10 | 30 :   			    ADC_CH0 | GPIOH_6
	-1,125,		// 11 | 31 :     			    GND | GPIOH_9
	-1,124,		// 12 | 32 :  			    ADC_CH2 | GPIOH_8
	-1,136,		// 13 | 33 :      			  SPDIF | GPIOAO_6
	-1,	-1,		// 14 | 34 : 			        GND | GND
   135,	-1,		// 15 | 35 : (GPIOAO_5)UART_RX_AO_B | PWM_AO_A(GPIOAO_3)
   134,	-1,		// 16 | 36 : (GPIOAO_4)UART_TX_AO_B | RTC_CLK
	-1,121,		// 17 | 37 :  		  			GND | GPIOH_5
   121,	-1,		// 18 | 38 :	 (GPIOAO_1)Linux_RX | PWR_EN
   122,	-1,		// 19 | 39 :     (GPIOAO_2)Linux_Tx | PWM_F
	-1,	-1,		// 20 | 40 : 				   3.3V | GND
	//  Not used
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
	-1,	-1,	-1,								//41-63
  };


/*------------------------------------------------------------------------------------------*/
/*																							*/
/*								Global variable define										*/
/*																							*/
/*------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------*/
/*								wiringPi Pinmap control arrary								*/
/*------------------------------------------------------------------------------------------*/
const int *pinToGpio, *phyToGpio;

/*	ADC file descriptor	*/
static char *adcFds[2];

/*	GPIO mmap control	*/
static volatile uint32_t *gpio, *gpio1;

/*	wiringPi Global library	*/
static struct libkhadas *lib = NULL;

/*------------------------------------------------------------------------------------------*/
/*									Function prototype define								*/
/*------------------------------------------------------------------------------------------*/
static int  gpioToGPSETReg  (int pin);
static int  gpioToGPLEVReg  (int pin);
static int  gpioToPUENReg   (int pin);
static int  gpioToPUPDReg   (int pin);
static int  gpioToShiftReg  (int pin);
static int  gpioToGPFSELReg (int pin);

/*------------------------------------------------------------------------------------------*/
/*									wiringPi core function									*/
/*------------------------------------------------------------------------------------------*/
static int      _getModeToGpio      (int mode, int pin);
static void     _pinMode        (int pin, int mode);
static int      _getAlt         (int pin);
static int      _getPUPD        (int pin);
static void     _pullUpDnControl    (int pin, int pud);
static int      _digitalRead        (int pin);
static void     _digitalWrite       (int pin, int value);
static int      _analogRead     (int pin);
static void     _digitalWriteByte   (const int value);
static unsigned int _digitalReadByte    (void);

/*------------------------------------------------------------------------------------------*/
/*									board init function										*/
/*------------------------------------------------------------------------------------------*/
static  int init_gpio_mmap (void);
static  void init_adc_fds   (void);
void init_khadas_vim1(struct libkhadas *libwiring);

/*------------------------------------------------------------------------------------------*/
/*							offset to the GPIO Set regsiter									*/
/*------------------------------------------------------------------------------------------*/
static int gpioToGPSETReg (int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return VIM1_GPIODV_OUTP_REG_OFFSET;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return VIM1_GPIOH_OUTP_REG_OFFSET;
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END)
		return VIM1_GPIOAO_OUTP_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*							offset to the GPIO Input regsiter								*/
/*------------------------------------------------------------------------------------------*/
static int gpioToGPLEVReg (int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return VIM1_GPIODV_INP_REG_OFFSET;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return VIM1_GPIOH_INP_REG_OFFSET;
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END)
		return VIM1_GPIOAO_INP_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*						offset to the GPIO Pull up/down enable regsiter						*/
/*------------------------------------------------------------------------------------------*/
static int gpioToPUENReg(int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return VIM1_GPIODV_PUEN_REG_OFFSET;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return VIM1_GPIOH_PUEN_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*							offset to the GPIO Pull up/down regsiter						*/
/*------------------------------------------------------------------------------------------*/
static int gpioToPUPDReg(int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return VIM1_GPIODV_PUPD_REG_OFFSET;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return VIM1_GPIOH_PUPD_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*							offset to the GPIO bit											*/
/*------------------------------------------------------------------------------------------*/
static int gpioToShiftReg (int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return pin - VIM1_GPIODV_PIN_START;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return pin - VIM1_GPIOH_PIN_START +20;
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END)
		return pin - VIM1_GPIOAO_PIN_START;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*							offset to the GPIO Function register							*/
/*------------------------------------------------------------------------------------------*/
static int gpioToGPFSELReg(int pin)
{
	if(pin >= VIM1_GPIODV_PIN_START && pin <= VIM1_GPIODV_PIN_END)
		return VIM1_GPIODV_FSEL_REG_OFFSET;
	if(pin >= VIM1_GPIOH_PIN_START && pin <= VIM1_GPIOH_PIN_END)
		return VIM1_GPIOH_FSEL_REG_OFFSET;
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END)
		return VIM1_GPIOAO_FSEL_REG_OFFSET;
	return -1;
}

static int _getModeToGpio(int mode, int pin)
{
	int retPin = -1;
	switch(mode){
		/* Native gpio number */
		case MODE_GPIO:
			retPin = pin;
			break;
		/* Native gpio number for sysfs */
		case MODE_GPIO_SYS:
			retPin = lib->sysFds[pin] != -1 ? pin : -1;
			break;
		/* wiringPi number */
		case MODE_PINS:
			retPin = pin < 64 ? pinToGpio[pin] : -1;
			break;
		/* header pin number */
		case MODE_PHYS:
			retPin = pin < 64 ? phyToGpio[pin] : -1;
			break;
		default:
			msg(MSG_WARN, "%s : Unknown Mode %d\n", __func__, mode);
			return -1;
	}

	/* To check I2C module loaded */
//	if(retPin == 207 || retPin == 208) {
//		if(moduleLoaded(AML_MODULE_I2C))
//			return -1;
//	}

	return retPin;
}

/*------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------*/
static void _pinMode(int pin, int mode)
{
	int fsel, shift, origPin = pin;

	if (lib->mode == MODE_GPIO_SYS)
		return;
	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return;

	softPwmStop  (origPin);
	softToneStop (origPin);

	fsel  = gpioToGPFSELReg(pin);
	shift = gpioToShiftReg (pin);
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
		switch (mode) {
			case INPUT:
				*(gpio1 + fsel) = (*(gpio1 + fsel) |  (1 << shift));
				break;
			case OUTPUT:
				*(gpio1 + fsel) = (*(gpio1 + fsel) & ~(1 << shift));
				break;
			case SOFT_PWM_OUTPUT:
				softPwmCreate (pin, 0, 100);
				break;
			case SOFT_TONE_OUTPUT:
				softToneCreate (pin);
				break;
			default:
				msg(MSG_WARN, "%s : Unknown Mode %d\n", __func__, mode);
				break;
		}
	}else{
		switch (mode) {
			case INPUT:
				*(gpio + fsel) = (*(gpio + fsel) |  (1 << shift));
				break;
			case OUTPUT:
				*(gpio + fsel) = (*(gpio + fsel) & ~(1 << shift));
				break;
			case SOFT_PWM_OUTPUT:
				softPwmCreate (pin, 0, 100);
				break;
			case SOFT_TONE_OUTPUT:
				softToneCreate (pin);
				break;
			default:
				msg(MSG_WARN, "%s : Unknown Mode %d\n", __func__, mode);
				break;
		}
	}
}

/*------------------------------------------------------------------------------------------*/
static int _getAlt (int pin)
{
	int fsel, shift;
	int mode = 0;

	if (lib->mode == MODE_GPIO_SYS)
		return 0;
	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return 2;

	fsel  = gpioToGPFSELReg(pin);
	shift = gpioToShiftReg(pin);

	switch(pin){
		case VIM1_GPIODV_PIN_START ...VIM1_GPIODV_PIN_END:
			switch(shift){
				case 24:
					if(*(gpio + VIM1_MUX_2_REG_OFFSET) & (1 << 16))		{ mode = 2; break; }
					if(*(gpio + VIM1_MUX_1_REG_OFFSET) & (1 << 15))     { mode = 4; break; }
					break;
				case 25:
					if(*(gpio + VIM1_MUX_2_REG_OFFSET) & (1 << 15))     { mode = 2; break; }
					if(*(gpio + VIM1_MUX_1_REG_OFFSET) & (1 << 14))     { mode = 4; break; }
					break;
				case 26:
					if(*(gpio + VIM1_MUX_2_REG_OFFSET) & (1 << 14))		{ mode = 2; break; }
					if(*(gpio + VIM1_MUX_1_REG_OFFSET) & (1 << 13))     { mode = 4; break; }
					break;
				case 27:
					if(*(gpio + VIM1_MUX_2_REG_OFFSET) & (1 << 13))     { mode = 2; break; }
					if(*(gpio + VIM1_MUX_1_REG_OFFSET) & (1 << 12))     { mode = 4; break; }
					break;
			}
			break;
		case VIM1_GPIOH_PIN_START ...VIM1_GPIOH_PIN_END:
			switch(shift){
				case 26:	//GPIOH6
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 26)) 	{ mode = 4; break; }
					break;
				case 27:	//GPIOH7
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 22)) 	{ mode = 4; break; }
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 25))		{ mode = 5; break; }
					break;
				case 28:	//GPIOH8
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 21)) 	{ mode = 4; break; }
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 24)) 	{ mode = 5; break; }
					break;
				case 29:	//GPIOH9
					if(*(gpio + VIM1_MUX_6_REG_OFFSET) & (1 << 23)) 	{ mode = 4; break; }
					break;
			}
			break;
		case VIM1_GPIOAO_PIN_START ...VIM1_GPIOAO_PIN_END:
			switch(shift){
				case 1:
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 11))	{ mode = 2; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 25))	{ mode = 3; break; }
					break;
				case 2:
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 10))	{ mode = 2; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 8))	{ mode = 3; break; }
					break;
				case 4:
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 24))	{ mode = 2; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 6))	{ mode = 3; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 2))	{ mode = 4; break; }
					break;
				case 5:
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 23))	{ mode = 2; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 5))	{ mode = 3; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 1))	{ mode = 4; break; }
					break;
				case 6:
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 16))	{ mode = 4; break; }
					if(*(gpio1 + VIM1_AO_MUX_1_REG_OFFSET) & (1 << 1))	{ mode = 5; break; }
					break;
			}
			break; 
		default:
			return -1;
	}
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
		return mode ? mode + 1 : (*(gpio1 + fsel) & (1 << shift)) ? 0 : 1;
	}else{
		return mode ? mode + 1 : (*(gpio + fsel) & (1 << shift)) ? 0 : 1;
	}
}

/*-------------------------------------------------------------------------------------------*/
static int _getPUPD(int pin)
{
	int puen, pupd, shift;

	if (lib->mode == MODE_GPIO_SYS)
		return -1;
	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return -1;

	puen  = gpioToPUENReg(pin);
	pupd  = gpioToPUPDReg(pin);
	shift = gpioToShiftReg(pin);
	
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
		return 0;
	}else{
		if (*(gpio + puen) & (1 << shift)){
			return *(gpio + pupd) & (1 << shift) ? 1 : 2;
		}
		else
			return 0;
	}

}

/*------------------------------------------------------------------------------------------*/
static void _pullUpDnControl(int pin, int pud)
{
	int shift = 0;
	if (lib->mode == MODE_GPIO_SYS)
		return;

	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return;
	
	 shift = gpioToShiftReg(pin);

	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
	
	}else{
		 if(pud){
	 		 // Enable Pull/Pull-down resister
		 	*(gpio + gpioToPUENReg(pin)) =
			 	(*(gpio + gpioToPUENReg(pin)) | (1 << shift));

			if (pud == PUD_UP)
				*(gpio + gpioToPUPDReg(pin)) =
					(*(gpio + gpioToPUPDReg(pin)) | (1 << shift));
			else
				*(gpio + gpioToPUPDReg(pin)) =
					(*(gpio + gpioToPUPDReg(pin)) & ~(1 << shift));
		 }else // Disable Pull/Pull-down resister
	 		*(gpio + gpioToPUENReg(pin)) =
				(*(gpio + gpioToPUENReg(pin)) & ~(1 << shift));
	}
}

/*------------------------------------------------------------------------------------------*/
static int _digitalRead(int pin)
{
	char c;

	if (lib->mode == MODE_GPIO_SYS) {
		if (lib->sysFds[pin] == -1)
			return LOW;

		lseek(lib->sysFds[pin], 0L, SEEK_SET);
		read(lib->sysFds[pin], &c, 1);

		return (c=='0') ? LOW : HIGH;
	}

	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return 0;
	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
		if ((*(gpio1 + gpioToGPLEVReg(pin)) & (1 << gpioToShiftReg(pin))) != 0)
			return HIGH;
		else
			return LOW;
	}else{
		if ((*(gpio + gpioToGPLEVReg(pin)) & (1 << gpioToShiftReg(pin))) != 0)
			return HIGH;
		else
			return LOW;
	}
}

/*------------------------------------------------------------------------------------------*/
static void _digitalWrite(int pin, int value)
{
	if (lib->mode == MODE_GPIO_SYS) {
		if (lib->sysFds[pin] != -1) {
			if (value == LOW)
				write (lib->sysFds[pin], "0\n", 2);
			else
				write (lib->sysFds[pin], "1\n", 2);
		}
		return;
	}

	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return;

	if(pin >= VIM1_GPIOAO_PIN_START && pin <= VIM1_GPIOAO_PIN_END){
		if (value == LOW)
			*(gpio1 + gpioToGPSETReg(pin)) &= ~(1 << (gpioToShiftReg(pin) + 16));
		else
			*(gpio1 + gpioToGPSETReg(pin)) |=  (1 << (gpioToShiftReg(pin) + 16));
	}else{
		if (value == LOW)
			*(gpio + gpioToGPSETReg(pin)) &= ~(1 << gpioToShiftReg(pin));
		else
			*(gpio + gpioToGPSETReg(pin)) |=  (1 << gpioToShiftReg(pin));
	}	
}


/*------------------------------------------------------------------------------------------*/
static int _analogRead (int pin)
{
	return -1;
}

/*------------------------------------------------------------------------------------------*/
static void _digitalWriteByte(const int value)
{
	return;
}
static unsigned int _digitalReadByte (void)
{
	return -1;
}

/*------------------------------------------------------------------------------------------*/

static int init_gpio_mmap(void)
{
	int fd;

	/* GPIO mmap setup */
	if(access("/dev/gpiomem",0) == 0){
		if ((fd = open ("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
			return msg (MSG_ERR,
				"wiringPiSetup: Unable to open /dev/gpiomem: %s\n",
				strerror (errno)) ;
	}else{
		if (geteuid () != 0)
			return msg (MSG_ERR,
				"wiringPiSetup: Must be root. (Did you forget sudo?)\n");
		
		if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
			return msg (MSG_ERR,
				"wiringPiSetup: Unable to open /dev/mem: %s\n",
				strerror (errno)) ;
	}
	gpio1  = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE,
					MAP_SHARED, fd, VIM1_GPIOAO_BASE);
	gpio  = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE,
					MAP_SHARED, fd, VIM1_GPIO_BASE);
	if (((int32_t)gpio == -1) || ((int32_t)gpio1 == -1))
		return msg (MSG_ERR,
			"wiringPiSetup: mmap (GPIO) failed: %s\n",
			strerror (errno));
}
/*------------------------------------------------------------------------------------------*/
static void init_adc_fds(void)
{

}
/*------------------------------------------------------------------------------------------*/
void init_khadas_vim1(struct libkhadas *libwiring)
{
	init_gpio_mmap();
	
	pinToGpio = pinToGpio_rev;
	phyToGpio = phyToGpio_rev;

	/* wiringPi core function initialize */
	libwiring->getModeToGpio    = _getModeToGpio;
	libwiring->pinMode      = _pinMode;
	libwiring->getAlt       = _getAlt;
	libwiring->getPUPD      = _getPUPD;
	libwiring->pullUpDnControl  = _pullUpDnControl;
	libwiring->digitalRead      = _digitalRead;
	libwiring->digitalWrite     = _digitalWrite;
	libwiring->analogRead       = _analogRead;
	libwiring->digitalWriteByte = _digitalWriteByte;
	libwiring->digitalReadByte  = _digitalReadByte;

	/* specify pin base number */
	libwiring->pinBase = VIM1_GPIO_PIN_BASE;

	/* global variable setup */
	lib = libwiring;
}

/*------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------*/
