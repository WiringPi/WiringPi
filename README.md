# WiringPi (Unofficial Fork)

This is an unofficial mirror/fork of wiringPi.  This repository
has become a mirror of the last "official" source release, plus a fork
facilitating updates to support newer hardware (primarily for use by
the ports) and fix bugs.

---

## Notable Updates

I've made various updates that I've wanted to see for a long time.

1. BCM pin numbering is now the default

The WiringPi pin numbering never made any sense to me. Probably because I never really got into Arduino.  
I modified the command line options to reflect:

```
$ gpio -h
Usage: gpio -v                Show version info
       gpio -h                Show Help
       gpio[-b|-p|-w] ...    Use bcm-gpio/physical/WiringPi pin numbering scheme.
                             If none specified, BCM GPIO numbering is used by default.
      [-x extension:params][[ -x ...]] ...
       gpio <mode/read/write/aread/awritewb/pwm/pwmTone/clock> ...
       gpio bank <bank>
       gpio <toggle/blink> <pin>
       gpio readall/allreadall
       gpio unexportall/exports
       gpio export/edge/unexport ...
       gpio wfi <pin> <mode>
       gpio mwfi <pin>[,<pin>...] <mode>
       gpio drive <group> <value>
       gpio pwm-bal/pwm-ms 
       gpio pwmr <range> 
       gpio pwmc <divider> 
       gpio load spi/i2c
       gpio unload spi/i2c
       gpio i2cd/i2cdetect
       gpio rbx/rbd
       gpio wb <value>
       gpio usbp high/low
```

2. Board version reporting

I'm keeping Gordon's copyright.  
Give credit where credit is due, since this would not exist without his work.

```
$ gpio -v

gpio version: 2.xx
Copyright (c) 2012-2018 Gordon Henderson
This is free software with ABSOLUTELY NO WARRANTY.
For details type: "gpio -warranty"

Raspberry Pi Details
  Type     : Pi 4B
  Processor: BCM2711
  Revision : 1.4
  Memory   : 8GB
  Maker    : Sony
  
  * Device tree is enabled.
  * Model string: "Raspberry Pi 4 Model B Rev 1.4"
  * This Raspberry Pi supports user-level GPIO access.
```

3. Overall pin reporting

I wanted the readall command to report the current state of pins, their alternate functions, and the names to go with them.
i.e. The pin names aren't static anymore.  
This works on the 3B and 4B. The old A and B should be the same as before. I don't have any of the other boards,
so I can't test on those.  
In the table below, note that I've changed pins 32 and 33 to the alt4 function to be able to use UART5:

```
$ gpio readall
 +-----+-----+------------+------+---+---Pi 4B--+---+------+------------+-----+-----+
 | BCM | wPi |     Name   | Mode | V | Physical | V | Mode |    Name    | wPi | BCM |
 +-----+-----+------------+------+---+----++----+---+------+------------+-----+-----+
 |     |     |       3.3v |      |   |  1 || 2  |   |      | 5v         |     |     |
 |   2 |   8 |       SDA1 | ALT0 | 1 |  3 || 4  |   |      | 5v         |     |     |
 |   3 |   9 |       SCL1 | ALT0 | 1 |  5 || 6  |   |      | GND        |     |     |
 |   4 |   7 |    GPIO.04 |  OUT | 0 |  7 || 8  | 1 | ALT5 | TXD1       | 15  | 14  |
 |     |     |        GND |      |   |  9 || 10 | 0 | ALT5 | RXD1       | 16  | 15  |
 |  17 |   0 |    GPIO.17 |  OUT | 0 | 11 || 12 | 0 |   IN | GPIO.18    | 1   | 18  |
 |  27 |   2 |    GPIO.27 |   IN | 1 | 13 || 14 |   |      | GND        |     |     |
 |  22 |   3 |    GPIO.22 |   IN | 0 | 15 || 16 | 0 |   IN | GPIO.23    | 4   | 23  |
 |     |     |       3.3v |      |   | 17 || 18 | 0 |   IN | GPIO.24    | 5   | 24  |
 |  10 |  12 |  SPI0_MOSI | ALT0 | 0 | 19 || 20 |   |      | GND        |     |     |
 |   9 |  13 |  SPI0_MISO | ALT0 | 0 | 21 || 22 | 0 |   IN | GPIO.25    | 6   | 25  |
 |  11 |  14 |  SPI0_SCLK | ALT0 | 0 | 23 || 24 | 1 |  OUT | GPIO.08    | 10  | 8   |
 |     |     |        GND |      |   | 25 || 26 | 1 |  OUT | GPIO.07    | 11  | 7   |
 |   0 |  30 |    GPIO.00 |  OUT | 0 | 27 || 28 | 1 |   IN | GPIO.01    | 31  | 1   |
 |   5 |  21 |    GPIO.05 |   IN | 1 | 29 || 30 |   |      | GND        |     |     |
 |   6 |  22 |    GPIO.06 |   IN | 1 | 31 || 32 | 1 | ALT4 | TXD5       | 26  | 12  |
 |  13 |  23 |       RXD5 | ALT4 | 1 | 33 || 34 |   |      | GND        |     |     |
 |  19 |  24 |    GPIO.19 |   IN | 1 | 35 || 36 | 1 |   IN | GPIO.16    | 27  | 16  |
 |  26 |  25 |    GPIO.26 |   IN | 1 | 37 || 38 | 1 |   IN | GPIO.20    | 28  | 20  |
 |     |     |        GND |      |   | 39 || 40 | 1 |   IN | GPIO.21    | 29  | 21  |
 +-----+-----+------------+------+---+----++----+---+------+------------+-----+-----+
 | BCM | wPi |     Name   | Mode | V | Physical | V | Mode |    Name    | wPi | BCM |
 +-----+-----+------------+------+---+---Pi 4B--+---+------+------------+-----+-----+
```

4. Added wiringPiISRmulti

This is like the existing wiringPiISR, but it takes a list of pins to wait for.  
Note that (for simplicity) the mode applies to all the specified pins.

---
