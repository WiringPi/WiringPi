/*
 * rp1_registers.h:
 *  Private header defining data structures used to interface directly
 *  with memory on RP1 Raspberry Pi GPIO hardware.
 *  For reference, refer to the RP1 Peripherals documentation:
 *    https://datasheets.raspberrypi.com/rp1/rp1-peripherals.pdf
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

#ifndef WIRINGPI_RP1_REGISTERS_H
#define WIRINGPI_RP1_REGISTERS_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define CHECK_OFFSET(struct_type, member, offset) static_assert(offsetof(struct_type, member) == offset, "Invalid offset for member " #member " of structure " #struct_type ". Should match " #offset ".");

typedef struct [[gnu::packed]] RP1_GPIO_IO_BANK {

  struct RP1_GPIO_IO_CHAN {  // Offset 0x00, Array of 28 by 0x08

    union {  // CHANx_STATUS; Offset 0x00 + x * 0x08
      const volatile uint32_t STATUS_register;
      struct {
        const volatile uint32_t                     : 8;  // Reserved
        const volatile uint32_t OUTFROMPERI         : 1;  // Output signal from selected peripheral, before register overide is applied
        const volatile uint32_t OUTTOPAD            : 1;  // Output signal to pad after register overide is applied
        const volatile uint32_t                     : 2;  // Reserved
        const volatile uint32_t OEFROMPERI          : 1;  // Output enable from selected peripheral, before register overide is applied
        const volatile uint32_t OETOPAD             : 1;  // Output enable to pad after register overide is applied
        const volatile uint32_t                     : 2;  // Reserved
        const volatile uint32_t INISDIRECT          : 1;  // Input signal from pad, goes directly to the selected peripheral without filtering or override
        const volatile uint32_t INFROMPAD           : 1;  // Input signal from pad, before filtering and override are applied
        const volatile uint32_t INFILTERED          : 1;  // Input signal from pad, after filtering is applied but before override, not valid if inisdirect=1
        const volatile uint32_t INTOPERI            : 1;  // Input signal to peripheral, after filtering and override are applied, not valid if inisdirect=1
        const volatile uint32_t EVENT_EDGE_LOW      : 1;  // Input pin has seen falling edge. Clear with ctrl_irqreset
        const volatile uint32_t EVENT_EDGE_HIGH     : 1;  // Input pin has seen rising edge. Clear with ctrl_irqreset
        const volatile uint32_t EVENT_LEVEL_LOW     : 1;  // Input pin is Low
        const volatile uint32_t EVENT_LEVEL_HIGH    : 1;  // Input pin is high
        const volatile uint32_t EVENT_F_EDGE_LOW    : 1;  // Input pin has seen a filtered falling edge. Clear with ctrl_irqreset
        const volatile uint32_t EVENT_F_EDGE_HIGH   : 1;  // Input pin has seen a filtered rising edge. Clear with ctrl_irqreset
        const volatile uint32_t EVENT_DB_LEVEL_LOW  : 1;  // Debounced input pin is low
        const volatile uint32_t EVENT_DB_LEVEL_HIGH : 1;  // Debounced input pin is high
        const volatile uint32_t IRQCOMBINED         : 1;  // Interrupt to processors, after masking
        const volatile uint32_t IRQTOPROC           : 1;  // Interrupt to processors, after mask and override is applied
        const volatile uint32_t                     : 2;  // Reserved
      };
    } STATUS;

    union {  // CHANx_CTRL; Offset 0x04 + 0x08 * x
      volatile uint32_t CTRL_register;
      struct {
        volatile uint32_t FUNCSEL : 5;                         // Function select. 31 == NULL. See GPIO function table for available functions.
        volatile uint32_t F_M     : 7;                         // Filter/debounce time constant M
        volatile enum RP1_GPIO_CHAN_CTRL_OUTOVER : uint32_t {  // OUTOVER - Output Override
          OUTOVER_FUNCSEL         = 0x0,                       //  Drive output from peripheral signal selected by funcsel
          OUTOVER_INVERSE_FUNCSEL = 0x1,                       //  Drive output from inverse of peripheral signal selected by funcsel
          OUTOVER_LOW             = 0x2,                       //  Drive output low
          OUTOVER_HIGH            = 0x3                        //  Drive output high
        } OUTOVER : 2;
        volatile enum RP1_GPIO_CHAN_CTRL_OEOVER : uint32_t {  // OEOVER - Output Enable Override
          OEOVER_FUNCSEL        = 0x0,                        //  Drive output enable from peripheral signal selected by funcsel
          OEOVER_INVERT_FUNCSEL = 0x1,                        //  Drive output enable from inverse of peripheral signal selected by funcsel
          OEOVER_DISABLE        = 0x2,                        //  Disable output
          OEOVER_ENABLE         = 0x3                         //  Enable output
        } OEOVER : 2;
        volatile enum RP1_GPIO_CHAN_CTRL_INOVER : uint32_t {  // INOVER - Input Override
          RP1_GPIO_CHAN_CTRL_INOVER_DEFAULT    = 0x0,         //  Don’t invert the peripheral input
          RP1_GPIO_CHAN_CTRL_INOVER_INVERT     = 0x1,         //  Invert the peripheral input
          RP1_GPIO_CHAN_CTRL_INOVER_DRIVE_LOW  = 0x2,         //  Drive peripheral input low
          RP1_GPIO_CHAN_CTRL_INOVER_DRIVE_HIGH = 0x3          //  Drive peripheral input high
        } INOVER                                : 2;
        volatile uint32_t                       : 2;           // Reserved
        volatile uint32_t IRQMASK_EDGE_LOW      : 1;           // Masks the edge low interrupt into the interrupt output
        volatile uint32_t IRQMASK_EDGE_HIGH     : 1;           // Masks the edge high interrupt into the interrupt output
        volatile uint32_t IRQMASK_LEVEL_LOW     : 1;           // Masks the level low interrupt into the interrupt output
        volatile uint32_t IRQMASK_LEVEL_HIGH    : 1;           // Masks the level high interrupt into the interrupt output
        volatile uint32_t IRQMASK_F_EDGE_LOW    : 1;           // Masks the filtered edge low interrupt into the interrupt output
        volatile uint32_t IRQMASK_F_EDGE_HIGH   : 1;           // Masks the filtered edge high interrupt into the interrupt output
        volatile uint32_t IRQMASK_DB_LEVEL_LOW  : 1;           // Masks the debounced level low interrupt into the interrupt output
        volatile uint32_t IRQMASK_DB_LEVEL_HIGH : 1;           // Masks the debounced level high interrupt into the interrupt output
        volatile uint32_t IRQRESET              : 1;           // Reset the interrupt edge detector
        volatile uint32_t                       : 1;           // Reserved
        volatile enum RP1_GPIO_CHAN_CTRL_IRQOVER : uint32_t {  // IRQOVER - Interrupt Override
          RP1_GPIO_CHAN_CTRL_IRQOVER_DEFAULT    = 0x0,         //  Don’t invert the interrupt
          RP1_GPIO_CHAN_CTRL_IRQOVER_INVERT     = 0x1,         //  Invert the input
          RP1_GPIO_CHAN_CTRL_IRQOVER_DRIVE_LOW  = 0x2,         //  Drive interrupt low
          RP1_GPIO_CHAN_CTRL_IRQOVER_DRIVE_HIGH = 0x3          //  Drive interrupt high
        } IRQOVER : 2;
      };
    } CTRL;
  } GPIO[28];

  union RP1_GPIO_IO_INTR {  // INTR : Raw Interrupts; Offset 0x100
    const volatile uint32_t INTR_register;
    struct {
      const volatile uint32_t GPIO0  : 1;
      const volatile uint32_t GPIO1  : 1;
      const volatile uint32_t GPIO2  : 1;
      const volatile uint32_t GPIO3  : 1;
      const volatile uint32_t GPIO4  : 1;
      const volatile uint32_t GPIO5  : 1;
      const volatile uint32_t GPIO6  : 1;
      const volatile uint32_t GPIO7  : 1;
      const volatile uint32_t GPIO8  : 1;
      const volatile uint32_t GPIO9  : 1;
      const volatile uint32_t GPIO10 : 1;
      const volatile uint32_t GPIO11 : 1;
      const volatile uint32_t GPIO12 : 1;
      const volatile uint32_t GPIO13 : 1;
      const volatile uint32_t GPIO14 : 1;
      const volatile uint32_t GPIO15 : 1;
      const volatile uint32_t GPIO16 : 1;
      const volatile uint32_t GPIO17 : 1;
      const volatile uint32_t GPIO18 : 1;
      const volatile uint32_t GPIO19 : 1;
      const volatile uint32_t GPIO20 : 1;
      const volatile uint32_t GPIO21 : 1;
      const volatile uint32_t GPIO22 : 1;
      const volatile uint32_t GPIO23 : 1;
      const volatile uint32_t GPIO24 : 1;
      const volatile uint32_t GPIO25 : 1;
      const volatile uint32_t GPIO26 : 1;
      const volatile uint32_t GPIO27 : 1;
      const volatile uint32_t        : 4;  // Reserved
    };
  } INTR;

  union RP1_GPIO_IO_PROC0_INTE {  // PROC0_INTE : Interrupt Enable for proc0; Offset 0x104
    volatile uint32_t PROC0_INTE_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PROC0_INTE;

  union RP1_GPIO_IO_PROC0_INTF {  // PROC0_INTF : Interrupt Force for proc0; Offset 0x108
    volatile uint32_t PROC0_INTF_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PROC0_INTF;

  union RP1_GPIO_IO_PROC0_INTS {  // PROC0_INTs : Interrupt status after masking & forcing for proc0; Offset 0x10C
    const volatile uint32_t PROC0_INTS_register;
    struct {
      const volatile uint32_t GPIO0  : 1;
      const volatile uint32_t GPIO1  : 1;
      const volatile uint32_t GPIO2  : 1;
      const volatile uint32_t GPIO3  : 1;
      const volatile uint32_t GPIO4  : 1;
      const volatile uint32_t GPIO5  : 1;
      const volatile uint32_t GPIO6  : 1;
      const volatile uint32_t GPIO7  : 1;
      const volatile uint32_t GPIO8  : 1;
      const volatile uint32_t GPIO9  : 1;
      const volatile uint32_t GPIO10 : 1;
      const volatile uint32_t GPIO11 : 1;
      const volatile uint32_t GPIO12 : 1;
      const volatile uint32_t GPIO13 : 1;
      const volatile uint32_t GPIO14 : 1;
      const volatile uint32_t GPIO15 : 1;
      const volatile uint32_t GPIO16 : 1;
      const volatile uint32_t GPIO17 : 1;
      const volatile uint32_t GPIO18 : 1;
      const volatile uint32_t GPIO19 : 1;
      const volatile uint32_t GPIO20 : 1;
      const volatile uint32_t GPIO21 : 1;
      const volatile uint32_t GPIO22 : 1;
      const volatile uint32_t GPIO23 : 1;
      const volatile uint32_t GPIO24 : 1;
      const volatile uint32_t GPIO25 : 1;
      const volatile uint32_t GPIO26 : 1;
      const volatile uint32_t GPIO27 : 1;
      const volatile uint32_t        : 4;  // Reserved
    };
  } PROC0_INTS;

  union RP1_GPIO_IO_PROC1_INTE {  // PROC1_INTE : Interrupt Enable for PROC1; Offset 0x110
    volatile uint32_t PROC1_INTE_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PROC1_INTE;

  union RP1_GPIO_IO_PROC1_INTF {  // PROC1_INTF : Interrupt Force for PROC1; Offset 0x114
    volatile uint32_t PROC1_INTF_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PROC1_INTF;

  union RP1_GPIO_IO_PROC1_INTS {  // PROC1_INTs : Interrupt status after masking & forcing for PROC1; Offset 0x118
    const volatile uint32_t PROC1_INTS_register;
    struct {
      const volatile uint32_t GPIO0  : 1;
      const volatile uint32_t GPIO1  : 1;
      const volatile uint32_t GPIO2  : 1;
      const volatile uint32_t GPIO3  : 1;
      const volatile uint32_t GPIO4  : 1;
      const volatile uint32_t GPIO5  : 1;
      const volatile uint32_t GPIO6  : 1;
      const volatile uint32_t GPIO7  : 1;
      const volatile uint32_t GPIO8  : 1;
      const volatile uint32_t GPIO9  : 1;
      const volatile uint32_t GPIO10 : 1;
      const volatile uint32_t GPIO11 : 1;
      const volatile uint32_t GPIO12 : 1;
      const volatile uint32_t GPIO13 : 1;
      const volatile uint32_t GPIO14 : 1;
      const volatile uint32_t GPIO15 : 1;
      const volatile uint32_t GPIO16 : 1;
      const volatile uint32_t GPIO17 : 1;
      const volatile uint32_t GPIO18 : 1;
      const volatile uint32_t GPIO19 : 1;
      const volatile uint32_t GPIO20 : 1;
      const volatile uint32_t GPIO21 : 1;
      const volatile uint32_t GPIO22 : 1;
      const volatile uint32_t GPIO23 : 1;
      const volatile uint32_t GPIO24 : 1;
      const volatile uint32_t GPIO25 : 1;
      const volatile uint32_t GPIO26 : 1;
      const volatile uint32_t GPIO27 : 1;
      const volatile uint32_t        : 4;  // Reserved
    };
  } PROC1_INTS;

  union RP1_GPIO_IO_PCIE_INTE {  // PCIE_INTE : Interrupt Enable for PCIE; Offset 0x11C
    volatile uint32_t PCIE_INTE_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PCIE_INTE;

  union RP1_GPIO_IO_PCIE_INTF {  // PCIE_INTF : Interrupt Force for PCIE; Offset 0x120
    volatile uint32_t PCIE_INTF_register;
    struct {
      volatile uint32_t GPIO0  : 1;
      volatile uint32_t GPIO1  : 1;
      volatile uint32_t GPIO2  : 1;
      volatile uint32_t GPIO3  : 1;
      volatile uint32_t GPIO4  : 1;
      volatile uint32_t GPIO5  : 1;
      volatile uint32_t GPIO6  : 1;
      volatile uint32_t GPIO7  : 1;
      volatile uint32_t GPIO8  : 1;
      volatile uint32_t GPIO9  : 1;
      volatile uint32_t GPIO10 : 1;
      volatile uint32_t GPIO11 : 1;
      volatile uint32_t GPIO12 : 1;
      volatile uint32_t GPIO13 : 1;
      volatile uint32_t GPIO14 : 1;
      volatile uint32_t GPIO15 : 1;
      volatile uint32_t GPIO16 : 1;
      volatile uint32_t GPIO17 : 1;
      volatile uint32_t GPIO18 : 1;
      volatile uint32_t GPIO19 : 1;
      volatile uint32_t GPIO20 : 1;
      volatile uint32_t GPIO21 : 1;
      volatile uint32_t GPIO22 : 1;
      volatile uint32_t GPIO23 : 1;
      volatile uint32_t GPIO24 : 1;
      volatile uint32_t GPIO25 : 1;
      volatile uint32_t GPIO26 : 1;
      volatile uint32_t GPIO27 : 1;
      volatile uint32_t        : 4;  // Reserved
    };
  } PCIE_INTF;

  union RP1_GPIO_IO_PCIE_INTS {  // PCIE_INTs : Interrupt status after masking & forcing for PCIE; Offset 0x124
    const volatile uint32_t PCIE_INTS_register;
    struct {
      const volatile uint32_t GPIO0  : 1;
      const volatile uint32_t GPIO1  : 1;
      const volatile uint32_t GPIO2  : 1;
      const volatile uint32_t GPIO3  : 1;
      const volatile uint32_t GPIO4  : 1;
      const volatile uint32_t GPIO5  : 1;
      const volatile uint32_t GPIO6  : 1;
      const volatile uint32_t GPIO7  : 1;
      const volatile uint32_t GPIO8  : 1;
      const volatile uint32_t GPIO9  : 1;
      const volatile uint32_t GPIO10 : 1;
      const volatile uint32_t GPIO11 : 1;
      const volatile uint32_t GPIO12 : 1;
      const volatile uint32_t GPIO13 : 1;
      const volatile uint32_t GPIO14 : 1;
      const volatile uint32_t GPIO15 : 1;
      const volatile uint32_t GPIO16 : 1;
      const volatile uint32_t GPIO17 : 1;
      const volatile uint32_t GPIO18 : 1;
      const volatile uint32_t GPIO19 : 1;
      const volatile uint32_t GPIO20 : 1;
      const volatile uint32_t GPIO21 : 1;
      const volatile uint32_t GPIO22 : 1;
      const volatile uint32_t GPIO23 : 1;
      const volatile uint32_t GPIO24 : 1;
      const volatile uint32_t GPIO25 : 1;
      const volatile uint32_t GPIO26 : 1;
      const volatile uint32_t GPIO27 : 1;
      const volatile uint32_t        : 4;  // Reserved
    };
  } PCIE_INTS;

} RP1_GPIO_IO_BANK;

CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[0].STATUS,  0x000);
CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[0].CTRL,    0x004);
CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[1].STATUS,  0x008);
CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[1].CTRL,    0x00C);
CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[27].STATUS, 0x0D8);
CHECK_OFFSET(RP1_GPIO_IO_BANK, GPIO[27].CTRL,   0x0DC);
CHECK_OFFSET(RP1_GPIO_IO_BANK, INTR,            0x100);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC0_INTE,      0x104);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC0_INTF,      0x108);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC0_INTS,      0x10C);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC1_INTE,      0x110);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC1_INTF,      0x114);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PROC1_INTS,      0x118);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PCIE_INTE,       0x11C);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PCIE_INTF,       0x120);
CHECK_OFFSET(RP1_GPIO_IO_BANK, PCIE_INTS,       0x124);

typedef struct [[gnu::packed]] RP1_GPIO_PADS_BANK {

  volatile enum RP1_GPIO_PADS_VOLTAGE_SELECT : uint32_t {  // Voltage select. Per bank control
    RP1_GPIO_PADS_VOLTAGE_3v3 = 0,
    RP1_GPIO_PADS_VOLTAGE_1v8 = 1
  } VOLTAGE_SELECT;

  union RP1_GPIO_PADS_CHAN {
    volatile uint32_t CTRL_register;                  // GPIOx_CTRL register
    struct {                                          // GPIOx_CTRL bitfields
      volatile uint32_t SLEWFAST : 1;                 // Slew rate control. 1 = Fast, 0 = Slow
      volatile uint32_t SCHMITT  : 1;                 // Enable schmitt trigger
      volatile uint32_t PDE      : 1;                 // Pull down enable
      volatile uint32_t PUE      : 1;                 // Pull up enable
      volatile enum RP1_GPIO_PADS_DRIVE : uint32_t {  // Drive strength
        RP1_GPIO_PADS_DRIVE_2mA,
        RP1_GPIO_PADS_DRIVE_4mA,
        RP1_GPIO_PADS_DRIVE_8mA,
        RP1_GPIO_PADS_DRIVE_12mA
      } DRIVE              : 2;
      volatile uint32_t IE : 1;   // Input enable
      volatile uint32_t OD : 1;   // Output disable
      volatile uint32_t    : 24;  // Reserved
    };
  } GPIO[28];

} RP1_GPIO_PADS_BANK;

CHECK_OFFSET(RP1_GPIO_PADS_BANK, VOLTAGE_SELECT,  0x00);
CHECK_OFFSET(RP1_GPIO_PADS_BANK, GPIO[0],         0x04);
CHECK_OFFSET(RP1_GPIO_PADS_BANK, GPIO[27],        0x70);

typedef struct [[gnu::packed]] RP1_PWM_BANK {

  union RP1_PWM_GLOBAL_CTRL {  // GLOBAL_CTRL
    volatile uint32_t GLOBAL_CTRL_register;
    struct {                              // GLOBAL_CTRL bitfields
      volatile uint32_t CHAN0_EN   : 1;   // Channel 0 Enable
      volatile uint32_t CHAN1_EN   : 1;   // Channel 1 Enable
      volatile uint32_t CHAN2_EN   : 1;   // Channel 2 Enable
      volatile uint32_t CHAN3_EN   : 1;   // Channel 3 Enable
      volatile uint32_t            : 27;  // Reserved
      volatile uint32_t SET_UPDATE : 1;   // Settings Update Trigger
                                          // To prevent mis-sampling of multi-bit bus signals in the PWM clock domain,
                                          // this bit should be used to trigger a settings update. This ensures that
                                          // all PWM channel settings update on the same PWM clock cycle.
                                          // Write 1 to trigger a settings update to the block. Self clears to 0.
                                          // This bit affects the chan*_en bits, chan*_phase, chan*_ctrl and common_range registers.
                                          // Writes to the *_duty and *_range registers have an integral update strobe and writes take effect on
                                          // the next counter overflow of the respective PWM channel.
    };
  } GLOBAL_CTRL;

  union RP1_PWM_FIFO_CTRL {  // FIFO_CTRL
    volatile uint32_t FIFO_CTRL_register;
    struct {
      volatile const uint32_t LEVEL      : 5;   // Number of available words in the FIFO
      volatile uint32_t       FLUSH      : 1;   // Assert to flush FIFO
      volatile const uint32_t FLUSH_DONE : 1;   // FIFO flush completed in the PWM clock domain
      volatile uint32_t                  : 4;   // Reserved
      volatile uint32_t THRESHOLD        : 5;   // Threshold for the comparator. DREQ is asserted when level <= threshold.
      volatile uint32_t DWELL_TIME       : 5;   // Delay in number of bus cycles before successive DREQs are generated. Used to account for system bus latency in write data arriving at the FIFO.
      volatile uint32_t                  : 10;  // Reserved
      volatile uint32_t DREQ_EN          : 1;   // 1: Generate DMA request signals to the DMA controller.
                                                // 0: Don’t generate request signals - the dreq_active interrupt is unaffected.
    };
  } FIFO_CTRL;

  volatile uint32_t COMMON_RANGE;  // Counter range register for channels that are set to use channel binding
  volatile uint32_t COMMON_DUTY;   // Counter compare register for channels that are set to use channel binding and are not set to use the common FIFO
  volatile uint32_t DUTY_FIFO;     // 32-bit interface to a 128-bit backed duty cycle FIFO.
                                   // In round-robin fashion, 32-bit writes to this address are sequentially packed
                                   // as 32*n-bit words that are pushed into the duty cycle FIFO. N varies as per the
                                   // number of enabled channels set to use the FIFO. A distributor checks which channels
                                   // are enabled and using the FIFO, and writes the 32-bit words accordingly.

  struct RP1_PWM_CHAN {
    union {  // CHANx_CTRL
      volatile uint32_t CTRL_register;
      struct {
        volatile enum RP1_PWM_CHAN_MODE : uint32_t {  // PWM generation mode
          RP1_PWM_CHAN_MODE_ZERO      = 0x00,         //  Generates 0
          RP1_PWM_CHAN_MODE_TRAILING  = 0x01,         //  Trailing-edge mark-space PWM modulation
          RP1_PWM_CHAN_MODE_PHASE     = 0x02,         //  Phase-correct mark-space PWM modulation
          RP1_PWM_CHAN_MODE_DENSITY   = 0x03,         //  Pulse-density encoded output
          RP1_PWM_CHAN_MODE_MSB       = 0x04,         //  MSB Serialiser output
          RP1_PWM_CHAN_MODE_MODERATED = 0x05,         //  Pulse position modulated output - a single high-pulse is transmitted per cycle
          RP1_PWM_CHAN_MODE_LEADING   = 0x06,         //  Leading-edge mark-space PWM modulation
          RP1_PWM_CHAN_MODE_LSB       = 0x07          //  LSB Serialiser output
        } MODE                    : 3;
        volatile uint32_t INVERT  : 1;         // Invert the output bit
        volatile uint32_t BIND    : 1;         // Bind Channel to the common_range and common_duty/duty_fifo registers
        volatile uint32_t USEFIFO : 1;         // Use duty_fifo instead of common_duty/chan_duty. Note: setting bind=0 and usefifo=1 will lead to unpredictable operation.
        volatile uint32_t SDM     : 1;         // Use sigma-delta noise shaping modulator. In conjunction with sdm_bitwidth,
                                               // treat the duty cycle as a 16-bit signed truncation of the 32- bit duty cycle value and quantise
                                               // to (sdm_bitwidth+1)-bits. The resulting quantisation noise is filtered using a 2nd-order loop.
        volatile uint32_t DITHER        : 1;   // When SDM mode is used, add a 1-bit LSB dither inside the noise shaping loop to suppress idle tones.
        volatile uint32_t FIFO_POP_MASK : 1;   // Counter overflow events generate FIFO pop events.
        volatile uint32_t               : 3;   // Reserved
        volatile uint32_t SDM_BITWIDTH  : 4;   // Quantise the 16-bit input to a (sdm_bitwidth+1)-bit output. 0 → 1-bit output.
        volatile uint32_t SDM_BIAS      : 16;  // Unsigned offset to be added to the output PWM code generated by the sigma-delta modulator.
      };
    };
    volatile uint32_t RANGE;  // Channel counter range.
    volatile uint32_t PHASE;  // Channel counter phase offset register.
                              // This register preloads the internal counter such that phase offsets between
                              // channels can be introduced. Do not set higher than the respective range register.
    volatile uint32_t DUTY;   // Channel counter compare register.d
  } CHAN[4];

  union RP1_PWM_INTR {  // INTR : Raw Interrupts
    volatile uint32_t INTR_register;
    struct {
      volatile uint32_t       FIFO_UNDERFLOW : 1;
      volatile uint32_t       FIFO_OVERFLOW  : 1;
      const volatile uint32_t FIFO_EMPTY     : 1;
      const volatile uint32_t FIFO_FULL      : 1;
      const volatile uint32_t DREQ_ACTIVE    : 1;
      volatile uint32_t       CHAN0_RELOAD   : 1;
      volatile uint32_t       CHAN1_RELOAD   : 1;
      volatile uint32_t       CHAN2_RELOAD   : 1;
      volatile uint32_t       CHAN3_RELOAD   : 1;
    };
  } INTR;

  union RP1_PWM_INTE {  // INTE : Interrupt Enable
    volatile uint32_t INTE_register;
    struct {
      volatile uint32_t FIFO_UNDERFLOW : 1;
      volatile uint32_t FIFO_OVERFLOW  : 1;
      volatile uint32_t FIFO_EMPTY     : 1;
      volatile uint32_t FIFO_FULL      : 1;
      volatile uint32_t DREQ_ACTIVE    : 1;
      volatile uint32_t CHAN0_RELOAD   : 1;
      volatile uint32_t CHAN1_RELOAD   : 1;
      volatile uint32_t CHAN2_RELOAD   : 1;
      volatile uint32_t CHAN3_RELOAD   : 1;
    };
  } INTE;

  union RP1_PWM_INTF {  // INTF : Interrupt Force
    volatile uint32_t INTF_register;
    struct {
      volatile uint32_t FIFO_UNDERFLOW : 1;
      volatile uint32_t FIFO_OVERFLOW  : 1;
      volatile uint32_t FIFO_EMPTY     : 1;
      volatile uint32_t FIFO_FULL      : 1;
      volatile uint32_t DREQ_ACTIVE    : 1;
      volatile uint32_t CHAN0_RELOAD   : 1;
      volatile uint32_t CHAN1_RELOAD   : 1;
      volatile uint32_t CHAN2_RELOAD   : 1;
      volatile uint32_t CHAN3_RELOAD   : 1;
    };
  } INTF;

  union RP1_PWM_INTS {  // INTS : Interrupt status after masking & forcing
    const volatile uint32_t INTS_register;
    struct {
      const volatile uint32_t FIFO_UNDERFLOW : 1;
      const volatile uint32_t FIFO_OVERFLOW  : 1;
      const volatile uint32_t FIFO_EMPTY     : 1;
      const volatile uint32_t FIFO_FULL      : 1;
      const volatile uint32_t DREQ_ACTIVE    : 1;
      const volatile uint32_t CHAN0_RELOAD   : 1;
      const volatile uint32_t CHAN1_RELOAD   : 1;
      const volatile uint32_t CHAN2_RELOAD   : 1;
      const volatile uint32_t CHAN3_RELOAD   : 1;
    };
  } INTS;

} RP1_PWM_BANK;

CHECK_OFFSET(RP1_PWM_BANK, GLOBAL_CTRL,           0x00);
CHECK_OFFSET(RP1_PWM_BANK, FIFO_CTRL,             0x04);
CHECK_OFFSET(RP1_PWM_BANK, COMMON_RANGE,          0x08);
CHECK_OFFSET(RP1_PWM_BANK, COMMON_DUTY,           0x0C);
CHECK_OFFSET(RP1_PWM_BANK, DUTY_FIFO,             0x10);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[0].CTRL_register, 0x14);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[0].RANGE,         0x18);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[0].PHASE,         0x1C);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[0].DUTY,          0x20);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[1].CTRL_register, 0x24);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[1].RANGE,         0x28);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[1].PHASE,         0x2C);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[1].DUTY,          0x30);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[2].CTRL_register, 0x34);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[2].RANGE,         0x38);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[2].PHASE,         0x3C);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[2].DUTY,          0x40);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[3].CTRL_register, 0x44);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[3].RANGE,         0x48);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[3].PHASE,         0x4C);
CHECK_OFFSET(RP1_PWM_BANK, CHAN[3].DUTY,          0x50);
CHECK_OFFSET(RP1_PWM_BANK, INTR,                  0x54);
CHECK_OFFSET(RP1_PWM_BANK, INTE,                  0x58);
CHECK_OFFSET(RP1_PWM_BANK, INTF,                  0x5C);
CHECK_OFFSET(RP1_PWM_BANK, INTS,                  0x60);

#undef CHECK_OFFSET

#endif  // WIRINGPI_BCM_REGISTERS_H
