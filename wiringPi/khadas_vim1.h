/*************************************************************************************/
/*                                                                                   */
/*                    WiringPi KHADAS_VIM1 Board Header file                         */
/*                                                                                   */
/*************************************************************************************/


/*-----------------------------------------------------------------------------------*/
#ifndef __KHADAS_VIM1_H__
#define __KHADAS_VIM1_H__

/*----------------------------------------------------------------------------------*/
#define VIM1_GPIO_MASK				(0xFFFFFF00)
#define VIM1_GPIO_BASE				0xc8834000
#define VIM1_GPIOAO_BASE			0xc8100000

#define VIM1_GPIO_PIN_BASE			100

#define VIM1_GPIODV_PIN_START		(VIM1_GPIO_PIN_BASE + 49)		//100+49=149
#define VIM1_GPIODV_PIN_END			(VIM1_GPIO_PIN_BASE + 78)		
#define VIM1_GPIOH_PIN_START		(VIM1_GPIO_PIN_BASE + 16)		//100+16=116
#define VIM1_GPIOH_PIN_END			(VIM1_GPIO_PIN_BASE + 25)
#define VIM1_GPIOAO_PIN_START		(VIM1_GPIO_PIN_BASE + 30)		//100+30=130
#define VIM1_GPIOAO_PIN_END			(VIM1_GPIO_PIN_BASE + 39)

#define VIM1_GPIODV_FSEL_REG_OFFSET			0x10C		//[29:0]
#define VIM1_GPIODV_OUTP_REG_OFFSET			0x10D		//[29:0]
#define VIM1_GPIODV_INP_REG_OFFSET			0x10E		//[29:0]
#define VIM1_GPIODV_PUPD_REG_OFFSET			0x13a		//[29:0]
#define VIM1_GPIODV_PUEN_REG_OFFSET			0x148		//[29:0]

#define VIM1_GPIOH_FSEL_REG_OFFSET			0x10F		//[29:20]
#define VIM1_GPIOH_OUTP_REG_OFFSET			0x110		//[29:20]
#define VIM1_GPIOH_INP_REG_OFFSET			0x111		//[29:20]
#define VIM1_GPIOH_PUPD_REG_OFFSET			0x13b		//[29:20]
#define VIM1_GPIOH_PUEN_REG_OFFSET			0x149		//[29:20]

#define VIM1_GPIOAO_FSEL_REG_OFFSET			0x009		//[9:0]
#define VIM1_GPIOAO_OUTP_REG_OFFSET			0x009		//[25:16]
#define VIM1_GPIOAO_INP_REG_OFFSET			0x00a		//[9:0]
#define VIM1_GPIOAO_PUPD_REG_OFFSET			
#define VIM1_GPIOAO_PUEN_REG_OFFSET


#define VIM1_MUX_0_REG_OFFSET			0x12C
#define VIM1_MUX_1_REG_OFFSET			0x12D
#define VIM1_MUX_2_REG_OFFSET			0x12E
#define VIM1_MUX_3_REG_OFFSET			0x12F
#define VIM1_MUX_4_REG_OFFSET			0x130
#define VIM1_MUX_5_REG_OFFSET			0x131
#define VIM1_MUX_6_REG_OFFSET			0x132
#define VIM1_MUX_7_REG_OFFSET			0x133
#define VIM1_MUX_8_REG_OFFSET			0x134
#define VIM1_MUX_9_REG_OFFSET			0x135

#define VIM1_AO_MUX_1_REG_OFFSET		0x005
#define VIM1_AO_MUX_2_REG_OFFSET		0x006

#ifdef __cplusplus
extern "C" {
#endif

extern void init_khadas_vim1(struct libkhadas *libwiring);

#ifdef __cpluscplus
}
#endif

#endif	/* __KHADAS_VIM1_H__ */



