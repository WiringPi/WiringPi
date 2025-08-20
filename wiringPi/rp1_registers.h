/*
 * rp1_registers.h:
 *  Private header defining data structures used to interface directly
 *  with memory on RP1 Raspberry Pi GPIO hardware.
 *  Copyright (c) 2025 Grazer Computer Club and contributors
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

#ifndef __WIRINGPI_RP1_REGISTERS_H__
#define __WIRINGPI_RP1_REGISTERS_H__

#include <stdint.h>

struct [[gnu::packed]] RP1_GPIO_IO_BANK {
  struct RP1_GPIO_IO_CHAN {
    union {                                               // CHANx_STATUS
      const volatile uint32_t STATUS_reg;                 // CHANx_STATUS register
      struct {                                            // CHANx_STATUS bitfields
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

    union {                                           // CHANx_CTRL
      volatile uint32_t CTRL_reg;                     // CHANx_CTRL register
      struct {                                        // CHANx_CTRL bitfields
        volatile uint32_t FUNCSEL : 5;                // Function select. 31 == NULL. See GPIO function table for available functions.
        volatile uint32_t F_M     : 7;                // Filter/debounce time constant M
        volatile enum RP1_GPIO_CHAN_CTRL_OUTOVER : uint32_t {
          OUTOVER_FUNCSEL         = 0x0,              //    Drive output from peripheral signal selected by funcsel
          OUTOVER_INVERSE_FUNCSEL = 0x1,              //    Drive output from inverse of peripheral signal selected by funcsel
          OUTOVER_LOW             = 0x2,              //    Drive output low
          OUTOVER_HIGH            = 0x3               //    Drive output high
        } OUTOVER : 2;                                // OUTOVER - Output Override
        volatile enum RP1_GPIO_CHAN_CTRL_OEOVER : uint32_t {
          OEOVER_FUNCSEL        = 0x0,                //    Drive output enable from peripheral signal selected by funcsel
          OEOVER_INVERT_FUNCSEL = 0x1,                //    Drive output enable from inverse of peripheral signal selected by funcsel
          OEOVER_DISABLE        = 0x2,                //    Disable output
          OEOVER_ENABLE         = 0x3                 //    Enable output
        } OEOVER : 2;                                 // OEOVER - Output Enable Override
        volatile enum RP1_GPIO_CHAN_CTRL_INOVER : uint32_t {
          INOVER_DEFAULT    = 0x0,                    //    Don’t invert the peripheral input
          INOVER_INVERT     = 0x1,                    //    Invert the peripheral input
          INOVER_DRIVE_LOW  = 0x2,                    //    Drive peripheral input low
          INOVER_DRIVE_HIGH = 0x3                     //    Drive peripheral input high
        } INOVER                                : 2;  // INOVER - Input Override
        volatile uint32_t                       : 2;  // Reserved
        volatile uint32_t IRQMASK_EDGE_LOW      : 1;  // Masks the edge low interrupt into the interrupt output
        volatile uint32_t IRQMASK_EDGE_HIGH     : 1;  // Masks the edge high interrupt into the interrupt output
        volatile uint32_t IRQMASK_LEVEL_LOW     : 1;  // Masks the level low interrupt into the interrupt output
        volatile uint32_t IRQMASK_LEVEL_HIGH    : 1;  // Masks the level high interrupt into the interrupt output
        volatile uint32_t IRQMASK_F_EDGE_LOW    : 1;  // Masks the filtered edge low interrupt into the interrupt output
        volatile uint32_t IRQMASK_F_EDGE_HIGH   : 1;  // Masks the filtered edge high interrupt into the interrupt output
        volatile uint32_t IRQMASK_DB_LEVEL_LOW  : 1;  // Masks the debounced level low interrupt into the interrupt output
        volatile uint32_t IRQMASK_DB_LEVEL_HIGH : 1;  // Masks the debounced level high interrupt into the interrupt output
        volatile uint32_t IRQRESET              : 1;  // Reset the interrupt edge detector
        volatile uint32_t                       : 1;  // Reserved
        volatile enum RP1_GPIO_CHAN_CTRL_IRQOVER : uint32_t {
          IRQOVER_DEFAULT    = 0x0,                   //    Don’t invert the interrupt
          IRQOVER_INVERT     = 0x1,                   //    Invert the input
          IRQOVER_DRIVE_LOW  = 0x2,                   //    Drive interrupt low
          IRQOVER_DRIVE_HIGH = 0x3                    //    Drive interrupt high
        } IRQOVER : 2;                                // IRQOVER - Interrupt Override
      };
    } CTRL;
  } GPIO[28];

  union RP1_GPIO_IO_INTR {  // INTR : Raw Interrupts
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
      const volatile uint32_t        : 5;  // Reserved
    };
  } INTR;

  union RP1_GPIO_IO_PROC0_INTE {  // PROC0_INTE : Interrupt Enable for proc0
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PROC0_INTE;

  union RP1_GPIO_IO_PROC0_INTF {  // PROC0_INTF : Interrupt Force for proc0
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PROC0_INTF;

  union RP1_GPIO_IO_PROC0_INTS {  // PROC0_INTs : Interrupt status after masking & forcing for proc0
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
      const volatile uint32_t        : 5;  // Reserved
    };
  } PROC0_INTS;

  union RP1_GPIO_IO_PROC1_INTE {  // PROC1_INTE : Interrupt Enable for PROC1
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PROC1_INTE;

  union RP1_GPIO_IO_PROC1_INTF {  // PROC1_INTF : Interrupt Force for PROC1
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PROC1_INTF;

  union RP1_GPIO_IO_PROC1_INTS {  // PROC1_INTs : Interrupt status after masking & forcing for PROC1
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
      const volatile uint32_t        : 5;  // Reserved
    };
  } PROC1_INTS;


    union RP1_GPIO_IO_PCIE_INTE {  // PCIE_INTE : Interrupt Enable for PCIE
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PCIE_INTE;

  union RP1_GPIO_IO_PCIE_INTF {  // PCIE_INTF : Interrupt Force for PCIE
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
      volatile uint32_t        : 5;  // Reserved
    };
  } PCIE_INTF;

  union RP1_GPIO_IO_PCIE_INTS {  // PCIE_INTs : Interrupt status after masking & forcing for PCIE
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
      const volatile uint32_t        : 5;  // Reserved
    };
  } PCIE_INTS;
};

struct [[gnu::packed]] RP1_GPIO_PADS_BANK {
  volatile enum RP1_GPIO_PADS_VOLTAGE_SELECT : uint32_t {
    RP1_GPIO_PADS_VOLTAGE_3v3 = 0,
    RP1_GPIO_PADS_VOLTAGE_1v8 = 1
  } VOLTAGE_SELECT;

  union RP1_GPIO_PADS_CHAN {
    volatile uint32_t CTRL_register;   // GPIOx_CTRL register
    struct {                           // GPIOx_CTRL bitfields
      volatile uint32_t SLEWFAST : 1;  // Slew rate control. 1 = Fast, 0 = Slow
      volatile uint32_t SCHMITT  : 1;  // Enable schmitt trigger
      volatile uint32_t PDE      : 1;  // Pull down enable
      volatile uint32_t PUE      : 1;  // Pull up enable
      volatile enum RP1_GPIO_PADS_DRIVE : uint32_t {
        DRIVE_2mA,
        DRIVE_4mA,
        DRIVE_8mA,
        DRIVE_12mA
      } DRIVE              : 2;   // Drive strength
      volatile uint32_t IE : 1;   // Input enable
      volatile uint32_t OD : 1;   // Output disable
      volatile uint32_t    : 24;  // Reserved
    };
  } GPIO[28];
};

#endif  // __WIRINGPI_BCM_REGISTERS_H__
