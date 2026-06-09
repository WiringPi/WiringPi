/*
 * bcm_registers.h:
 *  Private header defining data structures used to interface directly
 *  with memory on Broadcom-based Raspberry Pi GPIO hardware.
 *  For reference, refer to the BCM2711 Arm Peripherals documentation:
 *    https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
 ***********************************************************************
 * This file is part of wiringPi:
 *    https://github.com/WiringPi/WiringPi
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifndef WIRINGPI_BCM_REGISTERS_H
#define WIRINGPI_BCM_REGISTERS_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define CHECK_OFFSET(struct_type, member, offset) static_assert(offsetof(struct_type, member) == offset, "Invalid offset for member " #member " of structure " #struct_type ". Should match " #offset ".");

typedef struct [[gnu::packed]] BCM_PWM_BANK {

  // Note: The channel numbers used differ from the peripherals documentation for the BCM283X / BCM2711 chips.
  //    WiringPi uses channel numbers 0 and 1, whereas the hardware documentation uses channel numbers 1 and 2.
  //    Structure members have been named using the WiringPi convention, but the Broadcom names are listed
  //    in parentheses in the comments.

  union BCM_PWM_CTRL_REGISTER {  // CTL : PWM Control; Offset 0x00
    volatile uint32_t CTRL_register;
    struct {
      volatile uint32_t CHAN0_ENABLE    : 1;   // Channel 0 Enable (PWEN1)
      volatile uint32_t CHAN0_MODE      : 1;   // Channel 0 Mode (MODE1)
      volatile uint32_t CHAN0_REPEAT    : 1;   // Channel 0 Repeat Last Data (RPTL1)
      volatile uint32_t CHAN0_SILENCE   : 1;   // Channel 0 Silence Bit (SBIT1)
      volatile uint32_t CHAN0_POLARITY  : 1;   // Channel 0 Polarity (POLA1)
      volatile uint32_t CHAN0_FIFO      : 1;   // Channel 0 Use FIFO (USEF1)
      volatile uint32_t CLEAR_FIFO      : 1;   // Clear FIFO (CLRF)
      volatile uint32_t CHAN0_MS_ENABLE : 1;   // Channel 0 M/S Enable (MSEN1)
      volatile uint32_t CHAN1_ENABLE    : 1;   // Channel 1 Enable (PWEN2)
      volatile uint32_t CHAN1_MODE      : 1;   // Channel 1 Mode (MODE2)
      volatile uint32_t CHAN1_REPEAT    : 1;   // Channel 1 Repeat Last Data (RPTL2)
      volatile uint32_t CHAN1_SILENCE   : 1;   // Channel 1 Silence Bit (SBIT2)
      volatile uint32_t CHAN1_POLARITY  : 1;   // Channel 1 Polarity (POLA2)
      volatile uint32_t CHAN1_FIFO      : 1;   // Channel 1 Use FIFO (USEF2)
      volatile uint32_t                 : 1;   // Reserved
      volatile uint32_t CHAN1_MS_ENABLE : 1;   // Channel 1 M/S Enable (MSEN2)
      volatile uint32_t                 : 16;  // Reserved
    };
    struct BCM_PWM_CTRL_CHAN_FIELDS {
      volatile uint8_t ENABLE    : 1;  // Enable (PWEN1/2)
      volatile uint8_t MODE      : 1;  // Mode (MODE1/2)
      volatile uint8_t REPEAT    : 1;  // Repeat Last Data (RPTL1/2)
      volatile uint8_t SILENCE   : 1;  // Silence Bit (SBIT1/2)
      volatile uint8_t POLARITY  : 1;  // Polarity (POLA1/2)
      volatile uint8_t USE_FIFO  : 1;  // Use FIFO (USEF1/2)
      volatile uint8_t           : 1;  // Reserved (really CLRF for CHAN[0], reserved for CHAN[1])
      volatile uint8_t MS_ENABLE : 1;  // M/S Enable (MSEN1/2)
    } CHAN[2];
  } CTRL;

  union BCM_PWM_STATUS_REGISTER {  // PWM Status (STA); Offset 0x04
    volatile uint32_t STATUS_register;
    struct {
      const volatile uint32_t ERR_FULL    : 1;  // FIFO Full Flag (FULL1)
      const volatile uint32_t ERR_EMPTY   : 1;  // FIFO Empty Flag (EMPT1)
      volatile uint32_t       ERR_WRITE   : 1;  // FIFO Write Error Flag (WERR1)
      volatile uint32_t       ERR_READ    : 1;  // FIFO Read Error Flag (RERR1)
      volatile uint32_t       CHAN0_GAP   : 1;  // Channel 0 Gap Occurred Flag (GAP1)
      volatile uint32_t       CHAN1_GAP   : 1;  // Channel 1 Gap Occurred Flag (GAP2)
      volatile uint32_t                   : 2;  // Reserved
      volatile uint32_t       ERR_BUS     : 1;  // Bus Error Flag
      const volatile uint32_t CHAN0_STATE : 1;  // Channel 0 State (STA1)
      const volatile uint32_t CHAN1_STATE : 1;  // Channel 1 State (STA2)
    };
  } STATUS;

  union {
    struct {                    // Individually named registers
      union BCM_PWM_DMA_CONF {  // PWM DMA Configuration (DMAC); Offset 0x08
        volatile uint32_t DMA_CONF_register;
        struct {
          volatile uint32_t DREQ   : 8;   // DMA Threshold for DREQ signal
          volatile uint32_t PANIC  : 8;   // DMA Threshold for PANIC signal
          volatile uint32_t        : 15;  // Reserved
          volatile uint32_t ENABLE : 1;   // DMA Enable
        };
      } DMA_CONF;

      volatile uint32_t : 32;  // Reserved: Padding; Offset 0x0C

      volatile uint32_t : 32;  // Padding: Overlaps with CHAN[0].RANGE; Offset 0x10
      volatile uint32_t : 32;  // Padding: Overlaps with CHAN[0].DATA; Offset 0x14

      volatile uint32_t FIFO_IN;  // Channel FIFO Input (FIF1); Offset 0x18

      volatile uint32_t : 32;  // Reserved: Padding; Offset 0x0C

      volatile uint32_t : 32;  // Padding: Overlaps with CHAN[1].RANGE; Offset 0x20
      volatile uint32_t : 32;  // Padding: Overlaps with CHAN[1].DATA; Offset 0x24
    };

    struct BCM_PWM_CHAN {
      volatile uint32_t : 32;   // Padding: (Overlaps with DMA_CONF / FIFO_IN); Offset 0x08/0x18
      volatile uint32_t : 32;   // Reserved: Padding; Offset 0x0C/0x1C
      volatile uint32_t RANGE;  // Channel Range (RNG1/2); Offset 0x10/0x20
      volatile uint32_t DATA;   // Channel Data (DAT1/2); Offset 0x14/0x24
    } CHAN[2];
  };

} BCM_PWM_BANK;

CHECK_OFFSET(BCM_PWM_BANK, CTRL,          0x00);
CHECK_OFFSET(BCM_PWM_BANK, CTRL.CHAN[0],  0x00);
CHECK_OFFSET(BCM_PWM_BANK, CTRL.CHAN[1],  0x01);
CHECK_OFFSET(BCM_PWM_BANK, STATUS,        0x04);
CHECK_OFFSET(BCM_PWM_BANK, DMA_CONF,      0x08);
CHECK_OFFSET(BCM_PWM_BANK, CHAN[0].RANGE, 0x10);
CHECK_OFFSET(BCM_PWM_BANK, CHAN[0].DATA,  0x14);
CHECK_OFFSET(BCM_PWM_BANK, FIFO_IN,       0x18);
CHECK_OFFSET(BCM_PWM_BANK, CHAN[1].RANGE, 0x20);
CHECK_OFFSET(BCM_PWM_BANK, CHAN[1].DATA,  0x24);

#undef CHECK_OFFSET

#endif // WIRINGPI_BCM_REGISTERS_H