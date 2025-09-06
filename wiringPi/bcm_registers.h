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

typedef struct [[gnu::packed]] BCM_AUXILLARY_UART_SPI_BANK {

  // Note: The SPI master numbers used differ from the peripherals documentation for the BCM283X / BCM2711 chips.
  //    WiringPi uses SPI 0 and 1, whereas the hardware documentation uses SPI 1 and 2.
  //    Structure members have been named using the WiringPi convention, but the Broadcom names are listed
  //    in parentheses in the comments.

  union BCM_AUX_IRQ_REGISTER {  // AUX_IRQ : Auxiliary Interrupt status; Offset 0x00
    const volatile uint32_t AUX_IRQ_register;
    struct {
      const volatile uint32_t UART : 1;  // If set the mini UART has an interrupt pending.
      const volatile uint32_t SPI0 : 1;  // If set the SPI0 module has an interrupt pending.
      const volatile uint32_t SPI1 : 1;  // If set the SPI1 module has an interrupt pending.
    };
  } IRQ;

  union BCM_AUX_ENABLES_REGISTER {  // AUX_ENABLES : Auxiliary enables; Offset 0x04
    volatile uint32_t AUX_ENABLES_register;
    struct {
      volatile uint32_t UART : 1;  // If set the mini UART has an interrupt pending.
      volatile uint32_t SPI0 : 1;  // If set the SPI1 module has an interrupt pending.
      volatile uint32_t SPI1 : 1;  // If set the SPI2 module has an interrupt pending.
    };
  } ENABLES;

  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x08
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x0C
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x10
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x14
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x18
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x1C
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x20
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x24
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x28
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x2C
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x30
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x34
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x38
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x3C

  struct BCM_AUX_MINI_UART {
    union BCM_AUX_MU_IO_DATA_REGISTER {  // AUX_MU_IO_REG : Mini UART I/O Data; Offset 0x40
      volatile uint32_t       IO_DATA_register;
      volatile uint32_t       TX       : 8;  // Requires LINE_CTRL.DLAB == 0. Data written is put in the transmit FIFO (Provided it is not full).
      volatile const uint32_t RX       : 8;  // Requires LINE_CTRL.DLAB == 0. Data read is taken from the receive FIFO (Provided it is not empty).
      volatile uint32_t       BAUD_LS8 : 8;  // Requires LINE_CTRL.DLAB == 1. Access to the LS 8 bits of the 16-bit baudrate register. Easier access to all 16 bits available via the BAUD register.
    } IO_DATA;

    union BCM_AUX_MU_INT_EN_REGISTER {  // AUX_MU_IER_REG : Mini UART Interrupt Enable; Offset 0x44
      volatile uint32_t INT_EN_register;
      struct {
        volatile uint32_t TX : 1;  // Requires LINE_CTRL.DLAB == 0. If this bit is set the interrupt line is asserted whenever the transmit FIFO is empty. If this bit is clear no transmit interrupts are generated.
        volatile uint32_t RX : 1;  // Requires LINE_CTRL.DLAB == 0. If this bit is set the interrupt line is asserted whenever the receive FIFO holds at least 1 byte. If this bit is clear no receive interrupts are generated.
      };
      volatile uint32_t BAUD_MS8 : 8;  // Requires LINE_CTRL.DLAB == 1. Access to the MS 8 bits of the 16-bit baudrate register. Easier access to all 16 bits available via the BAUD register.
    } INT_EN;

    union BCM_AUX_MU_INT_ID_REGISTER {  // AUX_MU_IIR_REG : Mini UART Interrupt Enable; Offset 0x48
      volatile uint32_t INT_ID_register;
      const struct {
        volatile const uint32_t NOT_PENDING : 1;  // This bit is clear whenever an interrupt is pending.
        volatile const enum BCM_AUX_MU_INT_ID : uint32_t {
          BCM_AUX_MU_INT_ID_NONE = 0b00,      // No interrupts
          BCM_AUX_MU_INT_ID_TX   = 0b01,      // Transmit holding register empty
          BCM_AUX_MU_INT_ID_RX   = 0b10       // Receiver holds valid byte
        } INT_ID                        : 2;  // On read this register shows the interrupt ID bit.
        volatile const uint32_t         : 1;  // Always read as zero as the mini UART has no timeout function.
        volatile const uint32_t         : 2;  // Always read as zero.
        volatile const uint32_t FIFO_EN : 2;  // Both bits always read as 1 as the FIFOs are always enabled.
      };
      struct {
        volatile const uint32_t : 1;  // Overlaps with NOT_PENDING
        volatile enum BCM_AUX_MU_INT_CLR : uint32_t {
          BCM_AUX_MU_INT_CLR_NONE = 0b00,
          BCM_AUX_MU_INT_CLR_RX   = 0b01,  // Writing with bit 1 set will clear the receive FIFO
          BCM_AUX_MU_INT_CLR_TX   = 0b10,  // Writing with bit 2 set will clear the transmit FIFO
          BCM_AUX_MU_INT_CLR_BOTH = 0b11   // Writing with both bits set will clear both the receive FIFO and transmit FIFO
        } INT_CLR               : 2;       // Clear the receive and/or transmit FIFO
        volatile const uint32_t : 1;       // Always read as zero as the mini UART has no timeout function.
        volatile const uint32_t : 2;       // Always read as zero.
        volatile const uint32_t : 2;       // Both bits always read as 1 as the FIFOs are always enabled.
      };
    } INT_ID;

    union BCM_AUX_MU_LINE_CTRL_REGISTER {  // AUX_MU_LCR_REG : Mini UART Line Control; Offset 0x4C
      volatile uint32_t LINE_CTRL_register;
      struct {
        volatile uint32_t DATA_SIZE_8_BIT : 1;  // If clear the UART works in 7-bit mode. If set the UART works in 8-bit mode.
        volatile uint32_t                 : 5;  // Reserved.
        volatile uint32_t BREAK           : 1;  // If set high the UART1_TX line is pulled low continuously. If held for at least 12 bits times that will indicate a break condition.
        volatile uint32_t DLAB            : 1;  // If set the first two Mini UART registers give access to the Baudrate register. During operation this bit must be cleared.
      };
    } LINE_CTRL;

    union BCM_AUX_MU_MODEM_CTRL_REGISTER {  // AUX_MU_MCR_REG : Mini UART Line Control; Offset 0x50
      volatile uint32_t MODEM_CTRL_register;
      struct {
        volatile uint32_t     : 1;  // Reserved.
        volatile uint32_t RTS : 1;  // If clear the UART1_RTS line is high. If set the UART1_RTS line is low. This bit is ignored if the RTS is used for auto-flow control. (See the Mini UART Extra Control register description.)
      };
    } MODEM_CTRL;

    union BCM_AUX_MU_LINE_STATUS_REGISTER {  // AUX_MU_LSR_REG : Mini UART Line Status; Offset 0x54
      volatile const uint32_t LINE_STATUS_register;
      struct {
        volatile const uint32_t READY    : 1;  // This bit is set if the receive FIFO holds at least 1 symbol.
        volatile const uint32_t OVERRUN  : 1;  // This bit is set if there was a receiver overrun. That is: one or more characters arrived whilst the receive FIFO was full.
                                               // The newly arrived characters have been discarded. This bit is cleared each time this register is read.
                                               // To do a non-destructive read of this overrun bit use the Mini UART Extra Status register.
        volatile const uint32_t          : 3;  // Reserved.
        volatile const uint32_t TX_EMPTY : 1;  // This bit is set if the transmit FIFO can accept at least one byte.
        volatile const uint32_t TX_IDLE  : 1;  // This bit is set if the transmit FIFO is empty and the transmitter is idle. (Finished shifting out the last bit).
      };
    } LINE_STATUS;

    union BCM_AUX_MU_MODEM_STATUS_REGISTER {  // AUX_MU_MSR_REG : Mini UART Modem Status; Offset 0x58
      volatile const uint32_t MODEM_STATUS_register;
      struct {
        volatile const uint32_t            : 4;  // This bit is set if the receive FIFO holds at least 1 symbol.
        volatile const uint32_t CTS_STATUS : 1;  // This bit is the inverse of the UART1_CTS input. If set the UART1_CTS pin is low. If clear the UART1_CTS pin is high.
      };
    } MODEM_STATUS;

    union BCM_AUX_MU_SCRATCH_REGISTER {  // AUX_MU_SCRATCH : Mini UART Scratch; Offset 0x5C
      volatile uint32_t SCRATCH_register;
      struct {
        volatile uint32_t SCRATCH : 8;  // A single byte of temporary storage.
      };
    } SCRATCH;

    union BCM_AUX_MU_EXTRA_CTRL_REGISTER {  // AUX_MU_CNTL_REG : Mini UART Extra Control; Offset 0x60
      volatile uint32_t EXTRA_CTRL_register;
      struct {
        volatile uint32_t RX_ENABLE  : 1;  // Receiver enable: If this bit is set the mini UART receiver is enabled.
        volatile uint32_t TX_ENABLE  : 1;  // Transmitter enable: If this bit is set the mini UART transmitter is enabled.
        volatile uint32_t RTS_ENABLE : 1;  // Enable receive Auto flow-control using RTS:
                                           //   If this bit is set the RTS line will de-assert if the receive FIFO reaches its 'auto flow' level.
                                           //   In fact the RTS line will behave as an RTR (Ready To Receive) line.
                                           //   If this bit is clear the RTS line is controlled by the AUX_MU_MCR_REG register bit 1.
        volatile uint32_t CTS_ENABLE : 1;  // Enable transmit Auto flow-control using CTS:
                                           //   If this bit is set the transmitter will stop if the CTS line is de-asserted.
                                           //   If this bit is clear the transmitter will ignore the status of the CTS line.
        volatile enum BCM_AUX_MU_EXTRA_CTRL_RTS_FLOW_LEVEL : uint32_t {
          BCM_AUX_MU_RTS_FLOW_LEVEL_3S = 0b00,   //   De-assert RTS when the receive FIFO has 3 empty spaces left.
          BCM_AUX_MU_RTS_FLOW_LEVEL_2S = 0b01,   //   De-assert RTS when the receive FIFO has 2 empty spaces left.
          BCM_AUX_MU_RTS_FLOW_LEVEL_1S = 0b10,   //   De-assert RTS when the receive FIFO has 1 empty space left.
          BCM_AUX_MU_RTS_FLOW_LEVEL_4S = 0b11    //   De-assert RTS when the receive FIFO has 4 empty spaces left.
        } RTS_FLOW_LEVEL                   : 2;  // RTS AUTO flow level: These two bits specify at what receiver FIFO level the RTS line is de-asserted in auto-flow mode.
        volatile uint32_t RTS_ASSERT_LEVEL : 1;  // RTS assert level: This bit allows one to invert the RTS auto flow operation polarity.
                                                 //   If set the RTS auto flow assert level is low*
                                                 //   If clear the RTS auto flow assert level is high*
        volatile uint32_t CTS_ASSERT_LEVEL : 1;  // CTS assert level: This bit allows one to invert the CTS auto flow operation polarity.
                                                 //   If set the CTS auto flow assert level is low*
                                                 //   If clear the CTS auto flow assert level is high*

      };
    } EXTRA_CTRL;

    union BCM_AUX_MU_EXTRA_STATUS_REGISTER {  // AUX_MU_STAT_REG : Mini UART Extra Status; Offset 0x64
      volatile const uint32_t EXTRA_STATUS_register;
      struct {
        volatile const uint32_t RX_AVAILABLE     : 1; // Symbol available: If this bit is set the mini UART receive FIFO contains at least 1 symbol. If this bit is clear the mini UART receiver FIFO is empty.
        volatile const uint32_t TX_AVAILABLE     : 1; // Space available: If this bit is set the mini UART transmitter FIFO can accept at least one more symbol. If this bit is clear the mini UART transmitter FIFO is full.
        volatile const uint32_t RX_IDLE          : 1; // Receiver is idle: If this bit is set the receiver is idle. If this bit is clear the receiver is busy. This bit can change unless the receiver is disabled.
        volatile const uint32_t TX_IDLE          : 1; // Transmitter is idle: If this bit is set the transmitter is idle. If this bit is clear the transmitter is busy.
        volatile const uint32_t RX_OVERRUN       : 1; // Receiver overrun: This bit is set if there was a receiver overrun. That is: one or more characters arrived whilst the receive FIFO was full and were discarded.
                                                      //   This bit is cleared each time the AUX_MU_LSR_REG register is read.
        volatile const uint32_t TX_FIFO_FULL     : 1; // Transmit FIFO is full: This is the inverse of bit 1 (RX_AVAILABLE).
        volatile const uint32_t RTS_STATUS       : 1; // RTS status: This bit shows the status of the UART1_RTS line.
        volatile const uint32_t CTS_STATUS       : 1; // CTS line: This bit shows the status of the UART1_CTS line.
        volatile const uint32_t TX_FIFO_EMPTY    : 1; // If this bit is set the transmitter FIFO is empty. Thus it can accept 8 symbols.
        volatile const uint32_t TX_DONE          : 1; // This bit is set if the transmitter is idle and the transmit FIFO is empty. It is a logic AND of bits 3 and 8 (TX_IDLE && TX_FIFO_EMPTY).
        volatile const uint32_t                  : 6; // Reserved.
        volatile const uint32_t RX_FIFO_FILL_LVL : 4; // Receive FIFO fill level: These bits shows how many symbols are stored in the receive FIFO. The value is in the range 0-8.
        volatile const uint32_t                  : 4; // Reserved.
        volatile const uint32_t TX_FIFO_FILL_LVL : 4; // Transmit FIFO fill level: These bits shows how many symbols are stored in the transmit FIFO. The value is in the range 0-8.
        volatile const uint32_t                  : 4; // Reserved.
      };
    } EXTRA_STATUS;

    union {  // AUX_MU_BAUD_REG : Mini UART Baudrate; Offset 0x68
      volatile uint32_t BAUD_register;
      volatile uint32_t BAUD : 16;  // Mini UART baudrate counter
    };

  } MINI_UART;

  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x6C
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x70
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x74
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x78
  volatile uint32_t : 32;  // Reserved: Padding; Offset 0x7C

  struct BCM_AUX_SPI {
    struct BCM_AUX_SPI_CTRL_REGISTERS {
      union {  // AUX_SPIx_CNTL0_REG : SPI x Control register 0; Offset 0x80/0xC0
        volatile uint32_t CTRL0_register;
        struct {
          volatile uint32_t SHIFT_LEN     : 6;  // Specifies the number of bits to shift. This field is ignored when using 'variable width' mode.
          volatile uint32_t SHIFT_OUT_DIR : 1;  // If 1 the data is shifted out starting with the MS bit (bit 31 or bit 23). If 0 the data is shifted out starting with the LS bit (bit 0).
          volatile uint32_t CLK_INVERT    : 1;  // If 1 the 'idle' clock line state is high. If 0 the 'idle' clock line state is low.
                                                // Changing this bit will immediately change the polarity of the SPI clock output.
                                                // It is recommended to not do this when the CS is active, as the connected devices will see this as a clock change.
          volatile uint32_t OUT_RISING    : 1;  // If 1 data is clocked out on the rising edge of the SPI clock. If 0 data is clocked out on the falling edge of the SPI clock.
          volatile uint32_t CLEAR_FIFO    : 1;  // If 1 the receive and transmit FIFOs are held in reset (and thus flushed). This bit should be 0 during normal operation.
          volatile uint32_t IN_RISING     : 1;  // If 1 data is clocked in on the rising edge of the SPI clock. If 0 data is clocked in on the falling edge of the SPI clock.
          volatile uint32_t ENABLE        : 1;  // Enables the SPI interface. Whilst disabled the FIFOs can still be written to or read from. This bit should be 1 during normal operation.
          volatile enum BCM_AUX_SPI_CTRL_DOUT_HOLD : uint32_t {
            BCM_AUX_SPI_CTRL_DOUT_HOLD_0C = 0b00,  //   No extra hold time
            BCM_AUX_SPI_CTRL_DOUT_HOLD_1C = 0b01,  //   1 system clock extra hold time
            BCM_AUX_SPI_CTRL_DOUT_HOLD_4C = 0b10,  //   4 system clocks extra hold time
            BCM_AUX_SPI_CTRL_DOUT_HOLD_7C = 0b11   //   7 system clocks extra hold time
          } DOUT_HOLD                    : 2;      // Controls the extra DOUT hold time in system clock cycles.
                                                   //   Because the interface runs off fast silicon the MOSI hold time against the clock will be very short. This can cause
                                                   //   considerable problems on SPI slaves. To make it easier for the slave to see the data the hold time of the MOSI out
                                                   //   against the SPI clock out is programmable.
          volatile uint32_t VAR_WIDTH    : 1;      // If 1, the SPI takes the shift length and the data from the TX FIFO instead of the SHIFT_LEN field.
                                                   //   In this mode the shift length is taken from the transmit FIFO.
                                                   //   The transmit data bits 28:24 are used as shift length and the data bits 23:0 are the actual transmit data.
                                                   //   If the option 'shift MS out first' is selected the first bit shifted out will be bit 23. The receive data will arrive as normal.
          volatile uint32_t VAR_CS       : 1;      // If 1 the SPI takes the CS pattern and the data from the TX FIFO. If 0 the SPI takes the CS pattern from the CHIP_SELECTS field of this register. Set this bit only if bit 14 (variable width) is also set.
                                                   //   This mode is used together with the variable width mode. In this mode the CS pattern is taken from the transmit FIFO.
                                                   //   The transmit data bits 31:29 are used as CS and the data bits 23:0 are the actual transmit data. This allows the CPU
                                                   //   to write to different SPI devices without having to change the CS bits. However the data length is limited to 24 bits.
          volatile uint32_t POST_INPUT   : 1;      // If set the SPI input works in post-input mode.
          volatile uint32_t CHIP_SELECTS : 3;      // The pattern output on the CS pins when active.
          volatile uint32_t SPEED        : 12;     // Sets the SPI clock speed. spi_clk_freq = system_clock_freq/2*(speed+1)
        };
      };
      union {  // AUX_SPIx_CNTL1_REG : SPI x Control register 1; Offset 0x84/0xC4
        volatile uint32_t CTRL1_register;
        struct {
          volatile uint32_t KEEP_INPUT   : 1;  // If 1 the receiver shift register is NOT cleared. Thus new data is concatenated to old data. If 0 the receiver shift register is cleared before each transaction.
                                               //   Setting the 'Keep input' bit will prevent the input shift register being cleared between transactions. However the
                                               //   contents of the shift register is still written to the receive FIFO at the end of each transaction. E.g. if you receive two 8-
                                               //   bit values 0x81 followed by 0x46 the receive FIFO will contain: 0x0081 in the first entry and 0x8146 in the second
                                               //   entry. This mode may save CPU time concatenating bits (4 bits followed by 12 bits).
          volatile uint32_t SHIFT_IN_DIR : 1;  // If 1 the data is shifted in starting with the MS bit (bit 15). If 0 the data is shifted in starting with the LS bit (bit 0).
          volatile uint32_t              : 4;  // Reserved.
          volatile uint32_t DONE_IRQ     : 1;  // If 1 the interrupt line is high when the interface is idle.
          volatile uint32_t TX_EMPTY_IRQ : 1;  // If 1 the interrupt line is high when the transmit FIFO is empty.
          volatile uint32_t CS_HIGH_TIME : 3;  // Additional SPI clock cycles where the CS is high.
                                               //   The SPI CS will always be high for at least 1 SPI clock cycle. Some SPI devices need more time to process the data.
                                               //   This field will set a longer CS-high time. So the actual CS high time is (CS_high_time + 1) (in SPI clock cycles).
        };
      };
    } CTRL;

    union BCM_AUX_SPI_STATUS_REGISTER {  // AUX_SPIx_STAT_REG : SPI x Status; Offset 0x88/0xC8
      volatile const uint32_t STATUS_register;
      struct {
        volatile const uint32_t BIT_COUNT     : 6;  // The number of bits still to be processed. Starts with 'shift-length' and counts down.
        volatile const uint32_t BUST          : 1;  // Indicates the module is busy transferring data.
        volatile const uint32_t RX_EMPTY      : 1;  // If 1 the receiver FIFO is empty. If 0 the receiver FIFO holds at least 1 data unit.
        volatile const uint32_t RX_FULL       : 1;  // If 1 the receiver FIFO is full. If 0 the receiver FIFO can accept at least 1 data unit.
        volatile const uint32_t TX_EMPTY      : 1;  // If 1 the transmit FIFO is empty. If 0 the transmit FIFO holds at least 1 data unit.
        volatile const uint32_t TX_FULL       : 1;  // If 1 the transmit FIFO is full. If 0 the transmit FIFO can accept at least 1 data unit.
        volatile const uint32_t               : 5;  // Reserved.
        volatile const uint32_t RX_FIFO_LEVEL : 4;  // The number of data units in the receive data FIFO.
        volatile const uint32_t               : 4;  // Reserved.
        volatile const uint32_t TX_FIFO_LEVEL : 4;  // The number of data units in the transmit data FIFO.
      };
    } STATUS;

    union {  // AUX_SPIx_PEEK_REG : SPI x Peek; Offset 0x8C/0xCC
      volatile const uint32_t PEEK_register;
      volatile const uint32_t PEEK : 16;  // Reads from this address will show the top entry from the receive FIFO, but the data is not taken from the FIFO.
                                          //   This provides a means of inspecting the data but not removing it from the FIFO.
    };

    volatile uint32_t : 32;  // Reserved: Padding; Offset 0x90/0xD0
    volatile uint32_t : 32;  // Reserved: Padding; Offset 0x94/0xD4
    volatile uint32_t : 32;  // Reserved: Padding; Offset 0x98/0xD8
    volatile uint32_t : 32;  // Reserved: Padding; Offset 0x9C/0xDC

    union BCM_AUX_SPI_IO_DATA_REGISTERS {  // AUX_SPIx_IO_REGy : SPI x Data Registers; Offset 0xA0-0xAC/0xE0-0xEC
      volatile uint32_t DATA_register;
      volatile uint32_t DATA : 16;
    } IO_DATA[4];

    union BCM_AUX_SPI_IO_DATA_TXHOLD_REGISTERS {  // AUX_SPIx_TXHOLD_REGy : SPI x Extended Data Registers; Offset 0xB0-0xBC/0xF0-0xFC
      volatile uint32_t TXHOLD_DATA_register;
      volatile uint32_t TXHOLD_DATA : 16;
    } IO_DATA_TXHOLD[4];

  } SPI[2];

} BCM_AUX_BANK;

CHECK_OFFSET(BCM_AUX_BANK, IRQ,                        0x00);
CHECK_OFFSET(BCM_AUX_BANK, ENABLES,                    0x04);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.IO_DATA,          0x40);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.INT_EN,           0x44);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.INT_ID,           0x48);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.LINE_CTRL,        0x4C);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.MODEM_CTRL,       0x50);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.LINE_STATUS,      0x54);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.MODEM_STATUS,     0x58);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.SCRATCH,          0x5C);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.EXTRA_CTRL,       0x60);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.EXTRA_STATUS,     0x64);
CHECK_OFFSET(BCM_AUX_BANK, MINI_UART.BAUD_register,    0x68);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].CTRL.CTRL0_register, 0x80);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].CTRL.CTRL1_register, 0x84);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].STATUS,              0x88);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].PEEK_register,       0x8C);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA[0],          0xA0);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA[1],          0xA4);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA[2],          0xA8);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA[3],          0xAC);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA_TXHOLD[0],   0xB0);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA_TXHOLD[1],   0xB4);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA_TXHOLD[2],   0xB8);
CHECK_OFFSET(BCM_AUX_BANK, SPI[0].IO_DATA_TXHOLD[3],   0xBC);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].CTRL.CTRL0_register, 0xC0);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].CTRL.CTRL1_register, 0xC4);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].STATUS,              0xC8);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].PEEK_register,       0xCC);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA[0],          0xE0);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA[1],          0xE4);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA[2],          0xE8);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA[3],          0xEC);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA_TXHOLD[0],   0xF0);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA_TXHOLD[1],   0xF4);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA_TXHOLD[2],   0xF8);
CHECK_OFFSET(BCM_AUX_BANK, SPI[1].IO_DATA_TXHOLD[3],   0xFC);

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