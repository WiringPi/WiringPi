#ifndef _WIRING_CPI_H
#define _WIRING_CPI_H

#ifdef CONFIG_CLOCKWORKPI_A04
#define GPIOA_BASE                         (0x0300B000)
#define GPIO_NUM                           (0x40)
#define GPIO_BASE_MAP                      (0x0300B000)
#define MEM_INFO                           (1024)
#define GPIOL_BASE                         (0x07022000)
#define GPIOL_BASE_MAP                     (0x07022000)  
#define MAP_SIZE_L                         (4096 * 1)
#define GPIO_PWM_OP			   (0x0300A000)
#endif

#ifdef CONFIG_CLOCKWORKPI_A06
#define GPIO0_BASE 							0xff720000
#define GPIO1_BASE 							0xff730000
#define GPIO2_BASE 							0xff780000
#define GPIO3_BASE 							0xff788000
#define GPIO4_BASE 							0xff790000
#define GPIO_NUM                          	(160)
#define GPIO_SWPORTA_DR_OFFSET 				0x00
#define GPIO_SWPORTA_DDR_OFFSET 			0x04
#define GPIO_EXT_PORTA_OFFSET				0x50
#define PMUGRF_BASE 	     0xff320000	 
#define GRF_BASE 					0xff77e000
#define CRU_BASE 					0xff760000
#define PMUCRU_BASE 	0xff750000
#define CRU_CLKGATE_CON31_OFFSET 			0x037c    //bit 3 4 5
#define PMUCRU_CLKGATE_CON1_OFFSET 			0x0104
#endif

#define MAP_SIZE_L                         (4*1024)
#define MAP_SIZE           MAP_SIZE_L
#define MAP_MASK           (MAP_SIZE - 1)

extern int CPi_set_gpio_mode(int pin, int mode);
extern int CPi_set_gpio_alt(int pin, int mode);
extern int CPi_get_gpio_mode(int pin);
extern void CPi_digitalWrite(int pin, int value);
extern int CPi_digitalRead(int pin);
extern void CPiSetupRaw(void);

#endif
