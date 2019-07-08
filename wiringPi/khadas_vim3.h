/*------------------------------------------------------------------------------------------*/
/*																							*/
/*					WiringPi KHADAS_VIM3 Board Headler file									*/
/*																							*/
/*------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------*/
#ifndef __HKADAS_VIM3_H__
#define __KHADAS_VIM3_H__

/*------------------------------------------------------------------------------------------*/
#define VIM3_GPIO_MASK					(0xFFFFFF00)
#define VIM3_GPIO_BASE					0xff634000
#define VIM3_GPIOAO_BASE				0xff800000

#define VIM3_GPIO_PIN_BASE				300

#define VIM3_GPIOA_PIN_START			(VIM3_GPIO_PIN_BASE ) 	 	//300
#define VIM3_GPIOA_PIN_END				(VIM3_GPIO_PIN_BASE + 15)
#define VIM3_GPIOH_PIN_START			(VIM3_GPIO_PIN_BASE + 20)	//300+20=320
#define VIM3_GPIOH_PIN_END				(VIM3_GPIO_PIN_BASE + 29)
#define VIM3_GPIOZ_PIN_START			(VIM3_GPIO_PIN_BASE + 30)	//300+30=330
#define VIM3_GPIOZ_PIN_END				(VIM3_GPIO_PIN_BASE + 45)
#define VIM3_GPIOAO_PIN_START			(VIM3_GPIO_PIN_BASE + 50)	//300+50=350
#define VIM3_GPIOAO_PIN_END				(VIM3_GPIO_PIN_BASE + 59)

#define VIM3_GPIOA_FSEL_REG_OFFSET		0x120
#define VIM3_GPIOA_OUTP_REG_OFFSET		0x121
#define VIM3_GPIOA_INP_REG_OFFSET		0x122
#define VIM3_GPIOA_PUPD_REG_OFFSET		0x13F
#define VIM3_GPIOA_PUEN_REG_OFFSET    	0x14D
#define VIM3_GPIOA_DS_REG_5A_OFFSET   	0x1D6
#define VIM3_GPIOA_MUX_D_REG_OFFSET   	0x1BD
#define VIM3_GPIOA_MUX_E_REG_OFFSET   	0x1BE


#define VIM3_GPIOH_FSEL_REG_OFFSET		0x119		
#define VIM3_GPIOH_OUTP_REG_OFFSET		0x11A
#define VIM3_GPIOH_INP_REG_OFFSET       0x11B
#define VIM3_GPIOH_PUPD_REG_OFFSET		0x13D
#define VIM3_GPIOH_PUEN_REG_OFFSET      0x14B
#define VIM3_GPIOH_DS_REG_3A_OFFSET   	0x1D4
#define VIM3_GPIOH_MUX_B_REG_OFFSET		0x1BB

#define VIM3_GPIOAO_FSEL_REG_OFFSET     0x009
#define VIM3_GPIOAO_OUTP_REG_OFFSET     0x00d       
#define VIM3_GPIOAO_INP_REG_OFFSET      0x00a		//'0' is set to output ,defalut is input
#define VIM3_GPIOAO_PUPD_REG_OFFSET		0x00b
#define VIM3_GPIOAO_PUEN_REG_OFFSET		0x00c
#define VIM3_GPIOAO_DS_REG_A_OFFSET		0x007
#define VIM3_GPIOAO_MUX_0_REG_OFFSET	0x005
#define VIM3_GPIOAO_MUX_1_REG_OFFSET	0x006


#define VIM3_GPIOZ_FSEL_REG_OFFSET		0x11C		
#define VIM3_GPIOZ_OUTP_REG_OFFSET		0x11D
#define VIM3_GPIOZ_INP_REG_OFFSET       0x11E
#define VIM3_GPIOZ_PUPD_REG_OFFSET		0x14C
#define VIM3_GPIOZ_PUEN_REG_OFFSET      0x13E
#define VIM3_GPIOZ_DS_REG_4_OFFSET   	0x1D5
#define VIM3_GPIOZ_MUX_B_REG_OFFSET		0x1B6

#ifdef __cplusplus
extern "C"{
#endif

extern void init_khadas_vim3(struct libkhadas *libwiring);

#ifdef __cplusplus
}
#endif

#endif /* __KHADAS_VIM3_H__ */

