/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2018 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://github.com/nuncio-bitis/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>

extern int wpMode;

#ifndef TRUE
#  define       TRUE    (1==1)
#  define       FALSE   (1==2)
#endif

/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 *********************************************************************************
 */

static void doReadallExternal (void)
{
  int pin;

  printf ("+------+---------+--------+\n");
  printf ("|  Pin | Digital | Analog |\n");
  printf ("+------+---------+--------+\n");

  for (pin = wiringPiNodes->pinBase; pin <= wiringPiNodes->pinMax; ++pin)
    printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin));

  printf ("+------+---------+--------+\n");
}


/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 *********************************************************************************
 */

// FSEL_INPT, FSEL_OUTP, FSEL_ALT5, FSEL_ALT4, FSEL_ALT0, FSEL_ALT1, FSEL_ALT2, FSEL_ALT3
//       000,       001,       010,       011,       100,       101,       110,       111
static char *alts[] =
{
  "IN", "OUT", "ALT5", "ALT4", "ALT0", "ALT1", "ALT2", "ALT3"
};

static int physToWpi[64] =
{
  -1,     // 0
  -1, -1, // 1, 2
   8, -1,
   9, -1,
   7, 15,
  -1, 16,
   0,  1,
   2, -1,
   3,  4,
  -1,  5,
  12, -1,
  13,  6,
  14, 10,
  -1, 11, // 25, 26
  30, 31,	// Actually I2C, but not used
  21, -1,
  22, 26,
  23, -1,
  24, 27,
  25, 28,
  -1, 29, // 39, 40

  -1, -1, // 41, 42
  -1, -1,
  -1, -1,
  -1, -1,
  -1, -1,
  17, 18,
  19, 20, // 53, 54
  -1, -1, -1, -1, -1, -1, -1, -1, -1
};

// This is ony used for power and ground pins & old abReadall().
static char *physNames[64] =
{
  NULL,
  "   3.3v", "5v     ",
  "  SDA.1", "5v     ",
  "  SCL.1", "GND    ",
  " GPCLK0", "TXD1   ",
  "    GND", "RXD1   ",
  "GPIO.17", "GPIO.18",
  "GPIO.27", "GND    ",
  "GPIO.22", "GPIO.23",
  "   3.3v", "GPIO.24",
  "   MOSI", "GND    ",
  "   MISO", "GPIO.25",
  "   SCLK", "CE0    ",
  "    GND", "CE1    ",
  "  SDA.0", "SCL.0  ",
  "GPIO.05", "GND    ",
  "GPIO.06", "GPIO.12",
  "GPIO.13", "GND    ",
  "GPIO.19", "GPIO.16",
  "GPIO.26", "GPIO.20",
  "    GND", "GPIO.21",
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
  "GPIO.28", "GPIO.29",
  "GPIO.30", "GPIO.31",
   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};

static const char *gpio_alt_names_2835[54*8] = {
//   FSEL_INPT, FSEL_OUTP, FSEL_ALT5   , FSEL_ALT4    , FSEL_ALT0   , FSEL_ALT1    , FSEL_ALT2   , FSEL_ALT3
//         000,       001,       010   ,       011    ,       100   ,       101    ,       110   ,       111
    "GPIO.00" , "GPIO.00", 0           , "AVEIN_VCLK" , "SDA0"      , "SA5"        , "PCLK"      , "AVEOUT_VCLK"   ,
    "GPIO.01" , "GPIO.01", 0           , "AVEIN_DSYNC", "SCL0"      , "SA4"        , "DE"        , "AVEOUT_DSYNC"  ,
    "GPIO.02" , "GPIO.02", 0           , "AVEIN_VSYNC", "SDA1"      , "SA3"        , "LCD_VSYNC" , "AVEOUT_VSYNC"  ,
    "GPIO.03" , "GPIO.03", 0           , "AVEIN_HSYNC", "SCL1"      , "SA2"        , "LCD_HSYNC" , "AVEOUT_HSYNC"  ,
    "GPIO.04" , "GPIO.04", "ARM_TDI"   , "AVEIN_VID0" , "GPCLK0"    , "SA1"        , "DPI_D0"    , "AVEOUT_VID0"   ,
    "GPIO.05" , "GPIO.05", "ARM_TDO"   , "AVEIN_VID1" , "GPCLK1"    , "SA0"        , "DPI_D1"    , "AVEOUT_VID1"   ,
    "GPIO.06" , "GPIO.06", "ARM_RTCK"  , "AVEIN_VID2" , "GPCLK2"    , "SOE_N_SE"   , "DPI_D2"    , "AVEOUT_VID2"   ,
    "GPIO.07" , "GPIO.07", 0           , "AVEIN_VID3" , "SPI0_CE1_N", "SWE_N_SRW_N", "DPI_D3"    , "AVEOUT_VID3"   ,
    "GPIO.08" , "GPIO.08", 0           , "AVEIN_VID4" , "SPI0_CE0_N", "SD0"        , "DPI_D4"    , "AVEOUT_VID4"   ,
    "GPIO.09" , "GPIO.09", 0           , "AVEIN_VID5" , "SPI0_MISO" , "SD1"        , "DPI_D5"    , "AVEOUT_VID5"   ,
    "GPIO.10" , "GPIO.10", 0           , "AVEIN_VID6" , "SPI0_MOSI" , "SD2"        , "DPI_D6"    , "AVEOUT_VID6"   ,
    "GPIO.11" , "GPIO.11", 0           , "AVEIN_VID7" , "SPI0_SCLK" , "SD3"        , "DPI_D7"    , "AVEOUT_VID7"   ,
    "GPIO.12" , "GPIO.12", "ARM_TMS"   , "AVEIN_VID8" , "PWM0"      , "SD4"        , "DPI_D8"    , "AVEOUT_VID8"   ,
    "GPIO.13" , "GPIO.13", "ARM_TCK"   , "AVEIN_VID9" , "PWM1"      , "SD5"        , "DPI_D9"    , "AVEOUT_VID9"   ,
    "GPIO.14" , "GPIO.14", "TXD1"      , "AVEIN_VID10", "TXD0"      , "SD6"        , "DPI_D10"   , "AVEOUT_VID10"  ,
    "GPIO.15" , "GPIO.15", "RXD1"      , "AVEIN_VID11", "RXD0"      , "SD7"        , "DPI_D11"   , "AVEOUT_VID11"  ,
    "GPIO.16" , "GPIO.16", "CTS1"      , "SPI1_CE2_N" , "FL0"       , "SD8"        , "DPI_D12"   , "CTS0"          ,
    "GPIO.17" , "GPIO.17", "RTS1"      , "SPI1_CE1_N" , "FL1"       , "SD9"        , "DPI_D13"   , "RTS0"          ,
    "GPIO.18" , "GPIO.18", "PWM0"      , "SPI1_CE0_N" , "PCM_CLK"   , "SD10"       , "DPI_D14"   , "I2CSL_SDA_MOSI",
    "GPIO.19" , "GPIO.19", "PWM1"      , "SPI1_MISO"  , "PCM_FS"    , "SD11"       , "DPI_D15"   , "I2CSL_SCL_SCLK",
    "GPIO.20" , "GPIO.20", "GPCLK0"    , "SPI1_MOSI"  , "PCM_DIN"   , "SD12"       , "DPI_D16"   , "I2CSL_MISO"    ,
    "GPIO.21" , "GPIO.21", "GPCLK1"    , "SPI1_SCLK"  , "PCM_DOUT"  , "SD13"       , "DPI_D17"   , "I2CSL_CE_N"    ,
    "GPIO.22" , "GPIO.22", 0           , "ARM_TRST"   , "SD0_CLK"   , "SD14"       , "DPI_D18"   , "SD1_CLK"       ,
    "GPIO.23" , "GPIO.23", 0           , "ARM_RTCK"   , "SD0_CMD"   , "SD15"       , "DPI_D19"   , "SD1_CMD"       ,
    "GPIO.24" , "GPIO.24", 0           , "ARM_TDO"    , "SD0_DAT0"  , "SD16"       , "DPI_D20"   , "SD1_DAT0"      ,
    "GPIO.25" , "GPIO.25", 0           , "ARM_TCK"    , "SD0_DAT1"  , "SD17"       , "DPI_D21"   , "SD1_DAT1"      ,
    "GPIO.26" , "GPIO.26", 0           , "ARM_TDI"    , "SD0_DAT2"  , "TE0"        , "DPI_D22"   , "SD1_DAT2"      ,
    "GPIO.27" , "GPIO.27", 0           , "ARM_TMS"    , "SD0_DAT3"  , "TE1"        , "DPI_D23"   , "SD1_DAT3"      ,
    "GPIO.28" , "GPIO.28", 0           , 0            , "SDA0"      , "SA5"        , "PCM_CLK"   , "FL0"           ,
    "GPIO.29" , "GPIO.29", 0           , 0            , "SCL0"      , "SA4"        , "PCM_FS"    , "FL1"           ,
    "GPIO.30" , "GPIO.30", "CTS1"      , 0            , "TE0"       , "SA3"        , "PCM_DIN"   , "CTS0"          ,
    "GPIO.31" , "GPIO.31", "RTS1"      , 0            , "FL0"       , "SA2"        , "PCM_DOUT"  , "RTS0"          ,
    "GPIO.32" , "GPIO.32", "TXD1"      , 0            , "GPCLK0"    , "SA1"        , "RING_OCLK" , "TXD0"          ,
    "GPIO.33" , "GPIO.33", "RXD1"      , 0            , "FL1"       , "SA0"        , "TE1"       , "RXD0"          ,
    "GPIO.34" , "GPIO.34", 0           , 0            , "GPCLK0"    , "SOE_N_SE"   , "TE2"       , "SD1_CLK"       ,
    "GPIO.35" , "GPIO.35", 0           , 0            , "SPI0_CE1_N", "SWE_N_SRW_N", 0           , "SD1_CMD"       ,
    "GPIO.36" , "GPIO.36", 0           , 0            , "SPI0_CE0_N", "SD0"        , "TXD0"      , "SD1_DAT0"      ,
    "GPIO.37" , "GPIO.37", 0           , 0            , "SPI0_MISO" , "SD1"        , "RXD0"      , "SD1_DAT1"      ,
    "GPIO.38" , "GPIO.38", 0           , 0            , "SPI0_MOSI" , "SD2"        , "RTS0"      , "SD1_DAT2"      ,
    "GPIO.39" , "GPIO.39", 0           , 0            , "SPI0_SCLK" , "SD3"        , "CTS0"      , "SD1_DAT3"      ,
    "GPIO.40" , "GPIO.40", "TXD1"      , "SPI2_MISO"  , "PWM0"      , "SD4"        , 0           , "SD1_DAT4"      ,
    "GPIO.41" , "GPIO.41", "RXD1"      , "SPI2_MOSI"  , "PWM1"      , "SD5"        , "TE0"       , "SD1_DAT5"      ,
    "GPIO.42" , "GPIO.42", "RTS1"      , "SPI2_SCLK"  , "GPCLK1"    , "SD6"        , "TE1"       , "SD1_DAT6"      ,
    "GPIO.43" , "GPIO.43", "CTS1"      , "SPI2_CE0_N" , "GPCLK2"    , "SD7"        , "TE2"       , "SD1_DAT7"      ,
    "GPIO.44" , "GPIO.44", 0           , "SPI2_CE1_N" , "GPCLK1"    , "SDA0"       , "SDA1"      , "TE0"           ,
    "GPIO.45" , "GPIO.45", 0           , "SPI2_CE2_N" , "PWM1"      , "SCL0"       , "SCL1"      , "TE1"           ,
    "GPIO.46" , "GPIO.46", "SPI2_CE1_N", 0            , "SDA0"      , "SDA1"       , "SPI0_CE0_N", 0               ,
    "GPIO.47" , "GPIO.47", "SPI2_CE0_N", 0            , "SCL0"      , "SCL1"       , "SPI0_MISO" , 0               ,
    "GPIO.48" , "GPIO.48", "SPI2_SCLK" , "ARM_TRST"   , "SD0_CLK"   , "FL0"        , "SPI0_MOSI" , "SD1_CLK"       ,
    "GPIO.49" , "GPIO.49", "SPI2_MOSI" , "ARM_RTCK"   , "SD0_CMD"   , "GPCLK0"     , "SPI0_SCLK" , "SD1_CMD"       ,
    "GPIO.50" , "GPIO.50", 0           , "ARM_TDO"    , "SD0_DAT0"  , "GPCLK1"     , "PCM_CLK"   , "SD1_DAT0"      ,
    "GPIO.51" , "GPIO.51", 0           , "ARM_TCK"    , "SD0_DAT1"  , "GPCLK2"     , "PCM_FS"    , "SD1_DAT1"      ,
    "GPIO.52" , "GPIO.52", 0           , "ARM_TDI"    , "SD0_DAT2"  , "PWM0"       , "PCM_DIN"   , "SD1_DAT2"      ,
    "GPIO.53" , "GPIO.53", 0           , "ARM_TMS"    , "SD0_DAT3"  , "PWM1"       , "PCM_DOUT"  , "SD1_DAT3" 
};

static const char *gpio_alt_names_2711[54*8] = {
//   FSEL_INPT, FSEL_OUTP, FSEL_ALT5     , FSEL_ALT4         , FSEL_ALT0   , FSEL_ALT1    , FSEL_ALT2   , FSEL_ALT3
//         000,       001,       010     ,       011         ,       100   ,       101    ,       110   ,       111
    "GPIO.00" , "GPIO.00", "SDA6"        , "TXD2"            , "SDA0"      , "SA5"        , "PCLK"      , "SPI3_CE0_N"    ,
    "GPIO.01" , "GPIO.01", "SCL6"        , "RXD2"            , "SCL0"      , "SA4"        , "DE"        , "SPI3_MISO"     ,
    "GPIO.02" , "GPIO.02", "SDA3"        , "CTS2"            , "SDA1"      , "SA3"        , "LCD_VSYNC" , "SPI3_MOSI"     ,
    "GPIO.03" , "GPIO.03", "SCL3"        , "RTS2"            , "SCL1"      , "SA2"        , "LCD_HSYNC" , "SPI3_SCLK"     ,
    "GPIO.04" , "GPIO.04", "SDA3"        , "TXD3"            , "GPCLK0"    , "SA1"        , "DPI_D0"    , "SPI4_CE0_N"    ,
    "GPIO.05" , "GPIO.05", "SCL3"        , "RXD3"            , "GPCLK1"    , "SA0"        , "DPI_D1"    , "SPI4_MISO"     ,
    "GPIO.06" , "GPIO.06", "SDA4"        , "CTS3"            , "GPCLK2"    , "SOE_N_SE"   , "DPI_D2"    , "SPI4_MOSI"     ,
    "GPIO.07" , "GPIO.07", "SCL4"        , "RTS3"            , "SPI0_CE1_N", "SWE_N_SRW_N", "DPI_D3"    , "SPI4_SCLK"     ,
    "GPIO.08" , "GPIO.08", "SDA4"        , "TXD4"            , "SPI0_CE0_N", "SD0"        , "DPI_D4"    , "I2CSL_CE_N"    ,
    "GPIO.09" , "GPIO.09", "SCL4"        , "RXD4"            , "SPI0_MISO" , "SD1"        , "DPI_D5"    , "I2CSL_SDI_MISO",
    "GPIO.10" , "GPIO.10", "SDA5"        , "CTS4"            , "SPI0_MOSI" , "SD2"        , "DPI_D6"    , "I2CSL_SDA_MOSI",
    "GPIO.11" , "GPIO.11", "SCL5"        , "RTS4"            , "SPI0_SCLK" , "SD3"        , "DPI_D7"    , "I2CSL_SCL_SCLK",
    "GPIO.12" , "GPIO.12", "SDA5"        , "TXD5"            , "PWM0_0"    , "SD4"        , "DPI_D8"    , "SPI5_CE0_N"    ,
    "GPIO.13" , "GPIO.13", "SCL5"        , "RXD5"            , "PWM0_1"    , "SD5"        , "DPI_D9"    , "SPI5_MISO"     ,
    "GPIO.14" , "GPIO.14", "TXD1"        , "CTS5"            , "TXD0"      , "SD6"        , "DPI_D10"   , "SPI5_MOSI"     ,
    "GPIO.15" , "GPIO.15", "RXD1"        , "RTS5"            , "RXD0"      , "SD7"        , "DPI_D11"   , "SPI5_SCLK"     ,
    "GPIO.16" , "GPIO.16", "CTS1"        , "SPI1_CE2_N"      , 0           , "SD8"        , "DPI_D12"   , "CTS0"          ,
    "GPIO.17" , "GPIO.17", "RTS1"        , "SPI1_CE1_N"      , 0           , "SD9"        , "DPI_D13"   , "RTS0"          ,
    "GPIO.18" , "GPIO.18", "PWM0_0"      , "SPI1_CE0_N"      , "PCM_CLK"   , "SD10"       , "DPI_D14"   , "SPI6_CE0_N"    ,
    "GPIO.19" , "GPIO.19", "PWM0_1"      , "SPI1_MISO"       , "PCM_FS"    , "SD11"       , "DPI_D15"   , "SPI6_MISO"     ,
    "GPIO.20" , "GPIO.20", "GPCLK0"      , "SPI1_MOSI"       , "PCM_DIN"   , "SD12"       , "DPI_D16"   , "SPI6_MOSI"     ,
    "GPIO.21" , "GPIO.21", "GPCLK1"      , "SPI1_SCLK"       , "PCM_DOUT"  , "SD13"       , "DPI_D17"   , "SPI6_SCLK"     ,
    "GPIO.22" , "GPIO.22", "SDA6"        , "ARM_TRST"        , "SD0_CLK"   , "SD14"       , "DPI_D18"   , "SD1_CLK"       ,
    "GPIO.23" , "GPIO.23", "SCL6"        , "ARM_RTCK"        , "SD0_CMD"   , "SD15"       , "DPI_D19"   , "SD1_CMD"       ,
    "GPIO.24" , "GPIO.24", "SPI3_CE1_N"  , "ARM_TDO"         , "SD0_DAT0"  , "SD16"       , "DPI_D20"   , "SD1_DAT0"      ,
    "GPIO.25" , "GPIO.25", "SPI4_CE1_N"  , "ARM_TCK"         , "SD0_DAT1"  , "SD17"       , "DPI_D21"   , "SD1_DAT1"      ,
    "GPIO.26" , "GPIO.26", "SPI5_CE1_N"  , "ARM_TDI"         , "SD0_DAT2"  , 0            , "DPI_D22"   , "SD1_DAT2"      ,
    "GPIO.27" , "GPIO.27", "SPI6_CE1_N"  , "ARM_TMS"         , "SD0_DAT3"  , 0            , "DPI_D23"   , "SD1_DAT3"      ,
    "GPIO.28" , "GPIO.28", "RGMII_MDIO"  , "MII_A_RX_ERR"    , "SDA0"      , "SA5"        , "PCM_CLK"   , 0               ,
    "GPIO.29" , "GPIO.29", "RGMII_MDC"   , "MII_A_TX_ERR"    , "SCL0"      , "SA4"        , "PCM_FS"    , 0               ,
    "GPIO.30" , "GPIO.30", "CTS1"        , "MII_A_CRS"       , 0           , "SA3"        , "PCM_DIN"   , "CTS0"          ,
    "GPIO.31" , "GPIO.31", "RTS1"        , "MII_A_COL"       , 0           , "SA2"        , "PCM_DOUT"  , "RTS0"          ,
    "GPIO.32" , "GPIO.32", "TXD1"        , "SD_CARD_PRES"    , "GPCLK0"    , "SA1"        , 0           , "TXD0"          ,
    "GPIO.33" , "GPIO.33", "RXD1"        , "SD_CARD_WRPROT"  , 0           , "SA0"        , 0           , "RXD0"          ,
    "GPIO.34" , "GPIO.34", "RGMII_IRQ"   , "SD_CARD_LED"     , "GPCLK0"    , "SOE_N_SE"   , 0           , "SD1_CLK"       ,
    "GPIO.35" , "GPIO.35", 0             , "RGMII_START_STOP", "SPI0_CE1_N", "SWE_N_SRW_N", 0           , "SD1_CMD"       ,
    "GPIO.36" , "GPIO.36", "MII_A_RX_ERR", "RGMII_RX_OK"     , "SPI0_CE0_N", "SD0"        , "TXD0"      , "SD1_DAT0"      ,
    "GPIO.37" , "GPIO.37", "MII_A_TX_ERR", "RGMII_MDIO"      , "SPI0_MISO" , "SD1"        , "RXD0"      , "SD1_DAT1"      ,
    "GPIO.38" , "GPIO.38", "MII_A_CRS"   , "RGMII_MDC"       , "SPI0_MOSI" , "SD2"        , "RTS0"      , "SD1_DAT2"      ,
    "GPIO.39" , "GPIO.39", "MII_A_COL"   , "RGMII_IRQ"       , "SPI0_SCLK" , "SD3"        , "CTS0"      , "SD1_DAT3"      ,
    "GPIO.40" , "GPIO.40", "TXD1"        , "SPI0_MISO"       , "PWM1_0"    , "SD4"        , 0           , "SD1_DAT4"      ,
    "GPIO.41" , "GPIO.41", "RXD1"        , "SPI0_MOSI"       , "PWM1_1"    , "SD5"        , 0           , "SD1_DAT5"      ,
    "GPIO.42" , "GPIO.42", "RTS1"        , "SPI0_SCLK"       , "GPCLK1"    , "SD6"        , 0           , "SD1_DAT6"      ,
    "GPIO.43" , "GPIO.43", "CTS1"        , "SPI0_CE0_N"      , "GPCLK2"    , "SD7"        , 0           , "SD1_DAT7"      ,
    "GPIO.44" , "GPIO.44", "SD_CARD_VOLT", "SPI0_CE1_N"      , "GPCLK1"    , "SDA0"       , "SDA1"      , 0               ,
    "GPIO.45" , "GPIO.45", "SD_CARD_PWR0", "SPI0_CE2_N"      , "PWM0_1"    , "SCL0"       , "SCL1"      , 0               ,
    "GPIO.46" , "GPIO.46", "SPI2_CE1_N"  , 0                 , "SDA0"      , "SDA1"       , "SPI0_CE0_N", 0               ,
    "GPIO.47" , "GPIO.47", "SPI2_CE0_N"  , 0                 , "SCL0"      , "SCL1"       , "SPI0_MISO" , 0               ,
    "GPIO.48" , "GPIO.48", "SPI2_SCLK"   , "ARM_TRST"        , "SD0_CLK"   , 0            , "SPI0_MOSI" , "SD1_CLK"       ,
    "GPIO.49" , "GPIO.49", "SPI2_MOSI"   , "ARM_RTCK"        , "SD0_CMD"   , "GPCLK0"     , "SPI0_SCLK" , "SD1_CMD"       ,
    "GPIO.50" , "GPIO.50", "SPI2_MISO"   , "ARM_TDO"         , "SD0_DAT0"  , "GPCLK1"     , "PCM_CLK"   , "SD1_DAT0"      ,
    "GPIO.51" , "GPIO.51", "SD_CARD_LED" , "ARM_TCK"         , "SD0_DAT1"  , "GPCLK2"     , "PCM_FS"    , "SD1_DAT1"      ,
    "GPIO.52" , "GPIO.52", 0             , "ARM_TDI"         , "SD0_DAT2"  , "PWM0_0"     , "PCM_DIN"   , "SD1_DAT2"      ,
    "GPIO.53" , "GPIO.53", 0             , "ARM_TMS"         , "SD0_DAT3"  , "PWM0_1"     , "PCM_DOUT"  , "SD1_DAT3"      ,
};

static const char **gpio_alt_names = gpio_alt_names_2711;

/*
 * readallPhys:
 *	Given a physical pin output the data on it and the next pin:
 *| BCM | wPi |   Name  | Mode | Val| Physical |Val | Mode | Name    | wPi | BCM |
 *********************************************************************************
 */

static void readallPhys (int physPin)
{
  int pin;

  // | BCM | WPi
  if (physPinToGpio (physPin) == -1)
    printf (" |     |    ");
  else
    printf (" | %3d | %3d", physPinToGpio (physPin), physToWpi[physPin]);

  // | Name | Mode | Value
  if (physToWpi[physPin] == -1)
  {
    printf (" | %10s", physNames[physPin]);
    printf (" |      |  ");
  }
  else
  {
    if (wpMode == WPI_MODE_GPIO)
      pin = physPinToGpio (physPin);
    else if (wpMode == WPI_MODE_PHYS)
      pin = physPin;
    else
      pin = physToWpi[physPin];

    printf (" | %10s", gpio_alt_names[pin*8 + getAlt(pin)]);

    printf (" | %4s", alts[getAlt (pin)]);
    printf (" | %d", digitalRead (pin));
  }

  // Pin numbers:
  printf (" | %2d", physPin);
  ++physPin;
  printf (" || %-2d", physPin);

  // Same, reversed
  if (physToWpi[physPin] == -1)
  {
    printf (" |   |     ");
    printf (" | %-10s", physNames[physPin]);
  }
  else
  {
    if (wpMode == WPI_MODE_GPIO)
      pin = physPinToGpio (physPin);
    else if (wpMode == WPI_MODE_PHYS)
      pin = physPin;
    else
      pin = physToWpi[physPin];

    printf (" | %d", digitalRead (pin));
    printf (" | %4s", alts[getAlt (pin)]);

    printf (" | %-10s", gpio_alt_names[pin*8 + getAlt(pin)]);
  }

  if (physToWpi[physPin] == -1)
    printf (" |     |    ");
  else
    printf (" | %-3d | %-3d", physToWpi[physPin], physPinToGpio (physPin));

  printf (" |\n");
}


/*
 * allReadall:
 *	Read all the pins regardless of the model. Primarily of use for
 *	the compute module, but handy for other fiddling...
 *********************************************************************************
 */

static void allReadall (void)
{
  int pin;

  printf ("+------+------+-------+      +------+------+-------+\n");
  printf ("| BCM  |      |       |      | BCM  |      |       |\n");
  printf ("| GPIO | Mode | Value |      | GPIO | Mode | Value |\n");
  printf ("+------+------+-------+      +------+------+-------+\n");

  for (pin = 0; pin < 27; ++pin)
  {
    printf ("| %3d  ", pin);
    printf ("| %-4s ", alts[getAlt (pin)]);
    printf ("| %s  ", digitalRead (pin) == HIGH ? "High" : "Low ");
    printf ("|      ");
    printf ("| %3d  ", pin + 27);
    printf ("| %-4s ", alts[getAlt (pin + 27)]);
    printf ("| %s  ", digitalRead (pin + 27) == HIGH ? "High" : "Low ");
    printf ("|\n");
  }

  printf ("+------+------+-------+      +------+------+-------+\n");

}


/*
 * abReadall:
 *	Read all the pins on the model A or B.
 *********************************************************************************
 */

void abReadall (int model, int rev)
{
  int pin;
  char *type;

  if (model == PI_MODEL_A)
    type = " A";
  else
    if (rev == PI_VERSION_2)
      type = "B2";
    else
      type = "B1";

  printf (" +-----+-----+---------+------+---+-Model %s-+---+------+---------+-----+-----+\n", type);
  printf (" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
  printf (" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
  for (pin = 1; pin <= 26; pin += 2)
    readallPhys (pin);

  if (rev == PI_VERSION_2) // B version 2
  {
    printf (" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
    for (pin = 51; pin <= 54; pin += 2)
      readallPhys (pin);
  }

  printf (" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
  printf (" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
  printf (" +-----+-----+---------+------+---+-Model %s-+---+------+---------+-----+-----+\n", type);
}


/*
 * piPlusReadall:
 *	Read all the pins on the model A+ or the B+ or actually, all 40-pin Pi's
 *********************************************************************************
 */

static void plus2header (int model)
{
  /**/ if (model == PI_MODEL_AP)
    printf (" +-----+-----+---------+------+---+---Pi A+--+---+------+---------+-----+-----+\n");
  else if (model == PI_MODEL_BP)
    printf (" +-----+-----+---------+------+---+---Pi B+--+---+------+---------+-----+-----+\n");
  else if (model == PI_MODEL_ZERO)
    printf (" +-----+-----+---------+------+---+-Pi Zero--+---+------+---------+-----+-----+\n");
  else if (model == PI_MODEL_ZERO_W)
    printf (" +-----+-----+---------+------+---+-Pi ZeroW-+---+------+---------+-----+-----+\n");
  else if (model == PI_MODEL_ZERO_2W)
    printf (" +-----+-----+---------+------+---+Pi Zero 2W+---+------+---------+-----+-----+\n");
  else if (model == PI_MODEL_2B)
    printf (" +-----+-----+------------+------+---+---Pi 2---+---+------+------------+-----+-----+\n");
  else if (model == PI_MODEL_3B)
    printf (" +-----+-----+------------+------+---+---Pi 3B--+---+------+------------+-----+-----+\n");
  else if (model == PI_MODEL_3BP)
    printf (" +-----+-----+------------+------+---+---Pi 3B--+---+------+------------+-----+-----+\n");
  else if (model == PI_MODEL_3AP)
    printf (" +-----+-----+------------+------+---+---Pi 3A--+---+------+------------+-----+-----+\n");
  else if (model == PI_MODEL_4B)
    printf (" +-----+-----+------------+------+---+---Pi 4B--+---+------+------------+-----+-----+\n");
  else if (model == PI_MODEL_400)
    printf (" +-----+-----+------------+------+---+---Pi 400-+---+------+------------+-----+-----+\n");
  else
    printf (" +-----+-----+------------+------+---+---Pi ?---+---+------+------------+-----+-----+\n");
}


static void piPlusReadall (int model)
{
  int pin;

  plus2header (model);

  printf (" | BCM | wPi |     Name   | Mode | V | Physical | V | Mode |    Name    | wPi | BCM |\n");
  printf (" +-----+-----+------------+------+---+----++----+---+------+------------+-----+-----+\n");
  for (pin = 1; pin <= 40; pin += 2)
    readallPhys (pin);
  printf (" +-----+-----+------------+------+---+----++----+---+------+------------+-----+-----+\n");
  printf (" | BCM | wPi |     Name   | Mode | V | Physical | V | Mode |    Name    | wPi | BCM |\n");

  plus2header (model);
}


/*
 * doReadall:
 *	Generic read all pins called from main program. Works out the Pi type
 *	and calls the appropriate function.
 *********************************************************************************
 */

void doReadall (void)
{
  int model, proc, rev, mem, maker, overVolted;

  if (wiringPiNodes != NULL)	// External readall
  {
    doReadallExternal ();
    return;
  }

  piBoardId (&model, &proc, &rev, &mem, &maker, &overVolted);

  if ((model == PI_MODEL_A) || (model == PI_MODEL_B))
  {
    gpio_alt_names = gpio_alt_names_2835;
    abReadall (model, rev);
  }
  else if ((model == PI_MODEL_BP)   || (model == PI_MODEL_AP)     ||
           (model == PI_MODEL_2B)   ||
           (model == PI_MODEL_3AP)  ||
           (model == PI_MODEL_3B)   || (model == PI_MODEL_3BP)    ||
           (model == PI_MODEL_4B)   || (model == PI_MODEL_400)    || (model == PI_MODEL_CM4) ||
           (model == PI_MODEL_ZERO) || (model == PI_MODEL_ZERO_W) || (model == PI_MODEL_ZERO_2W))
  {
    gpio_alt_names = gpio_alt_names_2711;
    piPlusReadall (model);
  }
  else if ((model == PI_MODEL_CM1) || (model == PI_MODEL_CM3) || (model == PI_MODEL_CM3P) )
  {
    allReadall ();
  }
  else
  {
    printf ("Oops - unable to determine board type... model: %d\n", model);
  }
}


/*
 * doAllReadall:
 *	Force reading of all pins regardless of Pi model
 *********************************************************************************
 */

void doAllReadall (void)
{
  allReadall ();
}


/*
 * doQmode:
 *	Query mode on a pin
 *********************************************************************************
 */

void doQmode (int argc, char *argv[])
{
  int pin;

  if (argc != 3)
  {
    fprintf (stderr, "Usage: %s qmode pin\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  pin = atoi (argv[2]);
  printf ("%s\n", alts[getAlt (pin)]);
}
