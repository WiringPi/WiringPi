/*----------------------------------------------------------------------------*/
/*

	WiringPi KHADAS-EDGE Board Header file

 */
/*----------------------------------------------------------------------------*/
#ifndef	__KHADAS_EDGE_H__
#define	__KHADAS_EDGE_H__

/*----------------------------------------------------------------------------*/
// Common mmap block size for KHADAS-EDGE GRF register
#define EDGE_GRF_BLOCK_SIZE		0xF000

// Common offset for GPIO registers from each GPIO bank's base address
#define EDGE_GPIO_CON_OFFSET		0x04		// GPIO_SWPORTA_DDR
#define EDGE_GPIO_SET_OFFSET		0x00		// GPIO_SWPORTA_DR
#define EDGE_GPIO_GET_OFFSET		0x50		// GPIO_EXT_PORTA

#define EDGE_FUNC_GPIO			0b00		// Bit for IOMUX GPIO mode

// GPIO{0, 1}
#define EDGE_PMUGRF_BASE			0xFF320000
#define EDGE_PMUGRF_IOMUX_OFFSET		0x0000		// GRF_GPIO0A_IOMUX
#define EDGE_PMUGRF_PUPD_OFFSET		0x0040		// PMUGRF_GPIO0A_P

// GPIO{2, 3, 4}
#define EDGE_GRF_BASE 			0xFF770000
#define EDGE_GRF_IOMUX_OFFSET		0xE000		// GRF_GPIO2A_IOMUX
#define EDGE_GRF_PUPD_OFFSET		0xE040		// GRF_GPIO2A_P

// Offset to control GPIO clock
// Make 31:16 bit HIGH to enable the writing corresponding bit
#define EDGE_PMUCRU_BASE			0xFF750000
#define EDGE_PMUCRU_GPIO_CLK_OFFSET	0x0104		// PMUCRU_CLKGATE_COEDGE

#define EDGE_CRU_BASE			0xFF760000
#define EDGE_CRU_GPIO_CLK_OFFSET		0x037C		// CRU_CLKGATE_CON31

#define EDGE_CLK_ENABLE			0b0
#define EDGE_CLK_DISABLE			0b1

// Only for Linux kernel for now. Edit to 0 for Android
#ifndef __ANDROID__
#define EDGE_GPIO_PIN_BASE		1000
#else
#define EDGE_GPIO_PIN_BASE		0
#endif

// GPIO1_A.	0,1,2,3,4,7
// GPIO1_B. 	0,1,2,3,4,5
// GPIO1_C.	2,4,5,6
// GPIO1_D.	0
#define EDGE_GPIO_1_BASE			0xFF730000

// GPIO2_C.	0_B,1_B
#define EDGE_GPIO_2_BASE			0xFF780000

// GPIO4_C.	5,6
// GPIO4_D.	0,4,5,6
#define EDGE_GPIO_4_BASE			0xFF790000

// Reserved
// GPIO{0, 3}
#define EDGE_GPIO_0_BASE			0xFF720000
#define EDGE_GPIO_3_BASE			0xFF788000

#ifdef __cplusplus
extern "C" {
#endif

extern void init_khadas_edge (struct libkhadas *libwiring);

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------*/
#endif	/* __KHADAS_EDGE_H__ */
/*----------------------------------------------------------------------------*/
