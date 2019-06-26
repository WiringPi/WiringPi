/*------------------------------------------------------------------------------------------*/
/*																							*/
/*					WiringPi KHADAS_VIM2 Board Headler file									*/
/*																							*/
/*------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------*/
#ifndef __HKADAS_VIM2_H__
#define __KHADAS_VIM2_H__

/*------------------------------------------------------------------------------------------*/
#define VIM2_GPIO_MASK					(0xFFFFFF00)
#define VIM2_GPIO_BASE					0xc8834000
#define VIM2_GPIOAO_BASE				0xc8100000

#define VIM2_GPIO_PIN_BASE				200

#define VIM2_GPIODV_PIN_START			(VIM2_GPIO_PIN_BASE + 49) 	//200+49=249
#define VIM2_GPIODV_PIN_END				(VIM2_GPIO_PIN_BASE + 78)
#define VIM2_GPIOH_PIN_START			(VIM2_GPIO_PIN_BASE + 16)	//200+16=216
#define VIM2_GPIOH_PIN_END				(VIM2_GPIO_PIN_BASE + 25)
#define VIM2_GPIOAO_PIN_START			(VIM2_GPIO_PIN_BASE + 30)	//200+30=230
#define VIM2_GPIOAO_PIN_END				(VIM2_GPIO_PIN_BASE + 39)

#define VIM2_GPIODV_FSEL_REG_OFFSET		0x10C
#define VIM2_GPIODV_OUTP_REG_OFFSET		0x10D
#define VIM2_GPIODV_INP_REG_OFFSET		0x10E
#define VIM2_GPIODV_PUPD_REG_OFFSET     0x13a
#define VIM2_GPIODV_PUEN_REG_OFFSET     0x148

#define VIM2_GPIOH_FSEL_REG_OFFSET		0x10F		//[29:20]
#define VIM2_GPIOH_OUTP_REG_OFFSET		0x110
#define VIM2_GPIOH_INP_REG_OFFSET       0x111
#define VIM2_GPIOH_PUPD_REG_OFFSET		0x13b
#define VIM2_GPIOH_PUEN_REG_OFFSET      0x149

#define VIM2_GPIOAO_FSEL_REG_OFFSET     0x009
#define VIM2_GPIOAO_OUTP_REG_OFFSET     0x009       //[25:16]
#define VIM2_GPIOAO_INP_REG_OFFSET      0x00a
#define VIM2_GPIOAO_PUPD_REG_OFFSET
#define VIM2_GPIOAO_PUEN_REG_OFFSET	


#define VIM2_MUX_0_REG_OFFSET 			0x12C
#define VIM2_MUX_1_REG_OFFSET           0x12D
#define VIM2_MUX_2_REG_OFFSET           0x12E
#define VIM2_MUX_3_REG_OFFSET           0x12F
#define VIM2_MUX_4_REG_OFFSET           0x130
#define VIM2_MUX_5_REG_OFFSET           0x131
#define VIM2_MUX_6_REG_OFFSET           0x132
#define VIM2_MUX_7_REG_OFFSET           0x133
#define VIM2_MUX_8_REG_OFFSET           0x134
#define VIM2_MUX_9_REG_OFFSET           0x135

#define VIM2_AO_MUX_1_REG_OFFSET        0x005
#define VIM2_AO_MUX_2_REG_OFFSET        0x006

#ifdef __cplusplus
extern "C"{
#endif

extern void init_khadas_vim2(struct libkhadas *libwiring);

#ifdef __cplusplus
}
#endif

#endif /* __KHADAS_VIM2_H__ */

