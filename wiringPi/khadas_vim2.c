/*--------------------------------------------------------------------------------------*/
/*																						*/
/*				WiringPi KHADAS VIM2 Board control file									*/
/*																						*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>
#include <sys/mman.h>
#include <sys/utsname.h>

#include "softPwm.h"
#include "softTone.h"
#include "wiringPi.h"
#include "khadas_vim2.h"

/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*								WiringPi gpio map define								*/
/*--------------------------------------------------------------------------------------*/

static const int pinToGpio_rev[64] = {
	//wiringPi number to native gpio number
	270,275,		//   0 | 1  :					  GPIODV_21 | GPIODV_26
	271,272,		//   2 | 3  :					  GPIODV_22 | GPIODV_22
	 -1,223,		//	 4 | 5  :					  			| GPIOH_7
	225, -1,		//	 6 | 7  :						GPIOH_9 |
	 -1, -1,		// 	 8 | 9  :								|
	224,236,		//  10 | 11 :						GPIOH_8 | GPIOAO_6
	 -1, -1,		//	12 | 13 :								|
	 -1, -1,		// 	14 | 15 :								|
	276, -1,		// 	16 | 17 :					  GPIODV_27 |
	 -1, -1,		//	18 | 19 :								|
	 -1,235,		//	20 | 21 :								| GPIOAO_5
	234, -1,		// 	22 | 23 :					   GPIOAO_4 |
	231,230, 		//	24 | 25 :					   GPIOAO_1 | GPIOAO_0
	 -1, -1,		//	26 | 27 :								|
	262, -1,		//	28 | 29 :					  GPIODV_13 |
	 -1, -1,		//	30 | 31 : 								|
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //32to47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, //48to63

};

static const int phyToGpio_rev[64] = {
	//physical header pin number to native gpio number
	 -1,				//	0
	 -1, -1,			//	 1 | 21 :						 5V | GND
	 -1, -1,			//	 2 | 22 :						 5V	| GPIODV_25
	 -1, -1,			//	 3 | 23 :					 USB_DM | GPIODV_24
	 -1, -1,			//	 4 | 24 :					 USB_DP | GND
	 -1,276,			//	 5 | 25 :						GND	| GPIODV_27
	270,275,			//	 6 | 26 :				  GPIODV_21	| GPIODV_26
	271, -1,			//	 7 | 27 :				  GPIODV_22 | 3.3V
	272, -1,			//	 8 | 28 :				  GPIODV_23	| GND
	 -1,223,			//	 9 | 29 :						GND	| GPIOH7
	 -1, -1,			//	10 | 30 :					   ADC0	| GPIOH6
	 -1,225,			//	11 | 31 :					   1.8V	| GPIOH9
	 -1,224,			//	12 | 32 :					   ADC1	| GPIOH8
	 -1,236,			//	13 | 33 :					  SPDIF	| GPIOAO_6
	 -1, -1,			//	14 | 34 :						GND	| GND
	235, -1,			//	15 | 35 :	 (GPIOAO_5)UART_RX_AO_B | GPIODV_29
	234, -1,			//	16 | 36 :	 (GPIOAO_4)UART_TX_AO_B | RTC_CLK
	 -1, -1,			//	17 | 37 :						GND	| GPIOH5
	231, -1,			//	18 | 38 :		 (GPIOAO_1)Linux_RX	| EXP_INT
	230,262,			//	19 | 39 :		 (GPIOAO_0)Linux_TX | GPIODV_13
	 -1, -1,			//	20 | 40 :					   3.3V	| GND
	 //Not used
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1,										//41-63
};

/*--------------------------------------------------------------------------------------*/
/*																						*/
/*								Global variable define									*/
/*																						*/
/*--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------*/
/*							wiringPi Pinmap control array								*/
/*--------------------------------------------------------------------------------------*/
const int *pinToGpio, *phyToGpio;

/*	ADC file descriptor	*/
static char *adcFds[2];

/*	GPIO mmap control	*/
static volatile uint32_t *gpio,*gpio1;

/* 	wiringPi Global library	*/
static struct libkhadas *lib = NULL;

/*--------------------------------------------------------------------------------------*/
/*								Function prototype define								*/
/*--------------------------------------------------------------------------------------*/

static int  gpioToGPSETReg  (int pin);
static int  gpioToGPLEVReg  (int pin);
static int  gpioToPUENReg   (int pin);
static int  gpioToPUPDReg   (int pin);
static int  gpioToShiftReg  (int pin);
static int  gpioToGPFSELReg (int pin);

/*--------------------------------------------------------------------------------------*/
/*								 wiringPi core function									*/
/*--------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------*/
/*								board init function										*/
/*--------------------------------------------------------------------------------------*/
static  int init_gpio_mmap (void);
static  void init_adc_fds   (void);
void init_khadas_vim2(struct libkhadas *libwiring);

/*--------------------------------------------------------------------------------------*/
/*							offset to the GPIO Set regsiter								*/
/*--------------------------------------------------------------------------------------*/
static int gpioToGPSETReg (int pin)
{
    if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
		return VIM2_GPIODV_OUTP_REG_OFFSET;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return VIM2_GPIOH_OUTP_REG_OFFSET;
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END)
		return VIM2_GPIOAO_OUTP_REG_OFFSET;
	return -1; 
}

/*------------------------------------------------------------------------------------------*/
/*                          offset to the GPIO Input regsiter                               */
/*------------------------------------------------------------------------------------------*/
static int gpioToGPLEVReg (int pin)
{
	if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
		return VIM2_GPIODV_INP_REG_OFFSET;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return VIM2_GPIOH_INP_REG_OFFSET;
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END)
		return VIM2_GPIOAO_INP_REG_OFFSET;
	return -1; 
}

/*------------------------------------------------------------------------------------------*/
/*                      offset to the GPIO Pull up/down enable regsiter                     */
/*------------------------------------------------------------------------------------------*/
static int gpioToPUENReg(int pin)
{
    if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
		return VIM2_GPIODV_PUEN_REG_OFFSET;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return VIM2_GPIOH_PUEN_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*                          offset to the GPIO Pull up/down regsiter                        */
/*------------------------------------------------------------------------------------------*/
static int gpioToPUPDReg(int pin)
{
    if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
		return VIM2_GPIODV_PUPD_REG_OFFSET;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return VIM2_GPIOH_PUPD_REG_OFFSET;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*                          offset to the GPIO bit                                          */
/*------------------------------------------------------------------------------------------*/
static int gpioToShiftReg (int pin)
{
    if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
		return pin - VIM2_GPIODV_PIN_START;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return pin - VIM2_GPIOH_PIN_START +20;
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END)
		return pin - VIM2_GPIOAO_PIN_START;
	return -1;
}

/*------------------------------------------------------------------------------------------*/
/*                          offset to the GPIO Function register                            */
/*------------------------------------------------------------------------------------------*/
static int gpioToGPFSELReg(int pin)
{
    if(pin >= VIM2_GPIODV_PIN_START && pin <= VIM2_GPIODV_PIN_END)
	    return VIM2_GPIODV_FSEL_REG_OFFSET;
	if(pin >= VIM2_GPIOH_PIN_START && pin <= VIM2_GPIOH_PIN_END)
		return VIM2_GPIOH_FSEL_REG_OFFSET;
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END)
		return VIM2_GPIOAO_FSEL_REG_OFFSET;
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

/*--------------------------------------------------------------------------------------*/

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
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){
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

/*--------------------------------------------------------------------------------------*/
static int _getAlt(int pin)
{
	int fsel, shift;
	int mode = 0;

	if(lib->mode == MODE_GPIO_SYS)
		return 0;
	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return 2;

	fsel = gpioToGPFSELReg(pin);
	shift = gpioToShiftReg(pin);

	switch(pin){
		case VIM2_GPIODV_PIN_START ...VIM2_GPIODV_PIN_END:
			switch(shift){
				case 13:
					if(*(gpio + VIM2_MUX_3_REG_OFFSET) & (1 << 7))     { mode = 2; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 29))    { mode = 5; break; }
					break;
				case 21:
					if(*(gpio + VIM2_MUX_3_REG_OFFSET) & (1 << 5))     { mode = 2; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 25))    { mode = 5; break; }
					break;
				case 22:
					if(*(gpio + VIM2_MUX_3_REG_OFFSET) & (1 << 5))     { mode = 2; break; }
					if(*(gpio + VIM2_MUX_2_REG_OFFSET) & (1 << 18))    { mode = 4; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 25))    { mode = 5; break; }
					break;
				case 23:
					if(*(gpio + VIM2_MUX_3_REG_OFFSET) & (1 << 5))     { mode = 2; break; }
					if(*(gpio + VIM2_MUX_2_REG_OFFSET) & (1 << 17))    { mode = 4; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 25))    { mode = 5; break; }
					break;
				case 26:
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 20))    { mode = 2; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 13))    { mode = 3; break; }
					if(*(gpio + VIM2_MUX_2_REG_OFFSET) & (1 << 14))    { mode = 4; break; }
					break;
				case 27:
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 18))    { mode = 2; break; }
					if(*(gpio + VIM2_MUX_1_REG_OFFSET) & (1 << 12))    { mode = 3; break; }
					if(*(gpio + VIM2_MUX_2_REG_OFFSET) & (1 << 13))    { mode = 4; break; }
					break;
			}
			break;
		case VIM2_GPIOH_PIN_START ...VIM2_GPIOH_PIN_END:
			switch(shift){
				case 27:
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 25))    { mode = 4; break; }
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 22))    { mode = 5; break; }
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 19))    { mode = 6; break; }
					break;
				case 28:
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 24))    { mode = 4; break; }
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 21))    { mode = 5; break; }
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 18))    { mode = 6; break; }
					break;
				case 29:	
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 23))    { mode = 4; break; }
					if(*(gpio + VIM2_MUX_6_REG_OFFSET) & (1 << 17))    { mode = 6; break; }
					break;
			}
			break;
		case VIM2_GPIOAO_PIN_START ...VIM2_GPIOAO_PIN_END:
			switch(shift){
				case 0:
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 12))    { mode = 2; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 26))    { mode = 3; break; }
					break;
				case 1:
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 11))    { mode = 2; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 25))    { mode = 3; break; }
					break;
				case 4:
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 24))    { mode = 2; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 6))     { mode = 3; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 2))     { mode = 4; break; }
					break;
				case 5:
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 23))    { mode = 2; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 5))     { mode = 3; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 1))     { mode = 4; break; }
					break;
				case 6:
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 16))    { mode = 4; break; }
					if(*(gpio1 + VIM2_AO_MUX_1_REG_OFFSET) & (1 << 18))    { mode = 5; break; }
					break;
			}
			break;
		default:
			return -1;
	}
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){	
		return mode ? mode + 1 : (*(gpio1 + fsel) & (1 << shift)) ? 0 : 1;
	}else{
		return mode ? mode + 1 : (*(gpio + fsel) & (1 << shift)) ? 0 : 1;
	}
}

/*------------------------------------------------------------------------------------------*/
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
	
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){
		return 0;
	}else{
		if(*(gpio + puen) & (1 << shift)){
			return *(gpio + pupd) & (1 << shift) ? 1 : 2;
		}else{
			return 0;
		}
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

	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){
	
	}else{
		if(pud){
			//Enable Pull/Pull-down resister
			*(gpio + gpioToPUENReg(pin)) =
				(*(gpio + gpioToPUENReg(pin)) | (1 << shift));

			if (pud == PUD_UP)
				*(gpio + gpioToPUPDReg(pin)) =
					(*(gpio + gpioToPUPDReg(pin)) | (1 << shift));

			else
				*(gpio + gpioToPUPDReg(pin)) =
					(*(gpio + gpioToPUPDReg(pin)) & ~(1 << shift));
		}else //Disable Pull/Pull-down resister
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
	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){
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
			if(value == LOW)
				write (lib->sysFds[pin], "0\n", 2);
			else
				 write (lib->sysFds[pin], "1\n", 2);
		}
		return;
	}

	if ((pin = _getModeToGpio(lib->mode, pin)) < 0)
		return;

	if(pin >= VIM2_GPIOAO_PIN_START && pin <= VIM2_GPIOAO_PIN_END){
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

/*-------------------------------------------------------------------------------------------*/
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
					strerror (errno));
	}else{
		if (geteuid () != 0)
			return msg (MSG_ERR,
					"wiringPiSetup: Must be root. (Did you forget sudo?)\n");

		if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0)
			return msg (MSG_ERR,
					"wiringPiSetup: Unable to open /dev/mem: %s\n",
					strerror (errno));
	}
	gpio1  = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE,
						MAP_SHARED, fd, VIM2_GPIOAO_BASE);
	gpio  = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE,
						MAP_SHARED, fd, VIM2_GPIO_BASE);
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
void init_khadas_vim2(struct libkhadas *libwiring)
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
	libwiring->pinBase = VIM2_GPIO_PIN_BASE;

	/* global variable setup */
	lib = libwiring;
}

/*------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------*/
