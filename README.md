# WiringPi Library
Welcome to the WiringPi Library, the highly performant GPIO access library for Raspberry Pi boards. This library is written in C and is designed to provide fast and efficient control of the GPIO pins by directly accessing the hardware registers using DMA. 

**Key Features:** 
- **Support:** WiringPi supports all Raspberry Pi Boards including Pi 5 ( :construction: On the Pi 5, only the GCLK functionality is currently not supported due to missing documentation of the RP1 chip).
- **High Performance:** By directly accessing the hardware registers, WiringPi ensures minimal latency and maximum performance for your GPIO operations.
- **Wide Adoption:** WiringPi is widely used in numerous projects, making it a reliable choice for your Raspberry Pi GPIO needs.

Whether you’re working on a simple LED blink project or a complex automation system, WiringPi provides the tools you need to get the job done efficiently.

## How to use

To compile programs with wiringPi Library, you need to include `wiringPi.h` as well as link against `wiringPi`:

```c
#include <wiringPi.h> // Include WiringPi library!

int main(void)
{
  // uses BCM numbering of the GPIOs and directly accesses the GPIO registers.
  wiringPiSetupGpio();

  // pin mode ..(INPUT, OUTPUT, PWM_OUTPUT, GPIO_CLOCK)
  // set pin 17 to input
  pinMode(17, INPUT);

  // pull up/down mode (PUD_OFF, PUD_UP, PUD_DOWN) => down
  pullUpDnControl(17, PUD_DOWN);

  // get state of pin 17
  int value = digitalRead(17);

  if (HIGH == value)
  {
    // your code
  }
}
```

To compile this code, link against wiringPi:

```sh
gcc -o myapp myapp.c -l wiringPi
```

Be sure to check out the [examples](./examples/), build them using Make:

```sh
cd examples
make <example-name | really-all>
```

The tool `gpio` can be used to set single pins as well as get the state of everything at once:

```
pi@wiringdemo:~ $ gpio readall
 +-----+-----+---------+------+---+---Pi 3B--+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 |     |     |    3.3v |      |   |  1 || 2  |   |      | 5v      |     |     |
 |   2 |   8 |   SDA.1 |   IN | 1 |  3 || 4  |   |      | 5v      |     |     |
 |   3 |   9 |   SCL.1 |   IN | 1 |  5 || 6  |   |      | 0v      |     |     |
 |   4 |   7 | GPIO. 7 |   IN | 0 |  7 || 8  | 0 | IN   | TxD     | 15  | 14  |
 |     |     |      0v |      |   |  9 || 10 | 1 | IN   | RxD     | 16  | 15  |
 |  17 |   0 | GPIO. 0 |   IN | 1 | 11 || 12 | 1 | IN   | GPIO. 1 | 1   | 18  |
 |  27 |   2 | GPIO. 2 |   IN | 1 | 13 || 14 |   |      | 0v      |     |     |
 |  22 |   3 | GPIO. 3 |   IN | 1 | 15 || 16 | 0 | IN   | GPIO. 4 | 4   | 23  |
 |     |     |    3.3v |      |   | 17 || 18 | 1 | IN   | GPIO. 5 | 5   | 24  |
 |  10 |  12 |    MOSI |   IN | 0 | 19 || 20 |   |      | 0v      |     |     |
 |   9 |  13 |    MISO |   IN | 0 | 21 || 22 | 1 | IN   | GPIO. 6 | 6   | 25  |
 |  11 |  14 |    SCLK |   IN | 0 | 23 || 24 | 1 | IN   | CE0     | 10  | 8   |
 |     |     |      0v |      |   | 25 || 26 | 0 | IN   | CE1     | 11  | 7   |
 |   0 |  30 |   SDA.0 |   IN | 1 | 27 || 28 | 1 | IN   | SCL.0   | 31  | 1   |
 |   5 |  21 | GPIO.21 |   IN | 0 | 29 || 30 |   |      | 0v      |     |     |
 |   6 |  22 | GPIO.22 |   IN | 0 | 31 || 32 | 1 | IN   | GPIO.26 | 26  | 12  |
 |  13 |  23 | GPIO.23 |   IN | 1 | 33 || 34 |   |      | 0v      |     |     |
 |  19 |  24 | GPIO.24 |   IN | 1 | 35 || 36 | 1 | IN   | GPIO.27 | 27  | 16  |
 |  26 |  25 | GPIO.25 |   IN | 1 | 37 || 38 | 1 | IN   | GPIO.28 | 28  | 20  |
 |     |     |      0v |      |   | 39 || 40 | 1 | IN   | GPIO.29 | 29  | 21  |
 +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+
 | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |
 +-----+-----+---------+------+---+---Pi 3B--+---+------+---------+-----+-----+
```


## Installing

You can either build it yourself or use the prebuilt binaries:

### From Source

1. create debian-package

```sh
# fetch the source
sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi

# build the package
./build debian
mv debian-template/wiringpi-3.0-1.deb .

# install it
sudo apt install ./wiringpi-3.0-1.deb
```


### Prebuilt Binaries

Grab the latest release from [here](https://github.com/WiringPi/WiringPi/releases).


Unzip/use the portable prebuilt verison:

```sh
# unzip the archive
tar -xfv wiringpi_3.0.tar.gz
```

Install the debian package:

```sh
# install a dpkg
sudo apt install ./wiringpi-3.0-1.deb
```


## Ports

wiringPi has been wrapped for multiple languages:

* Node - https://github.com/WiringPi/WiringPi-Node
* Perl - https://github.com/WiringPi/WiringPi-Perl
* PHP - https://github.com/WiringPi/WiringPi-PHP
* Python - https://github.com/WiringPi/WiringPi-Python
* Ruby - https://github.com/WiringPi/WiringPi-Ruby

## Support

Please use the [issue system](https://github.com/WiringPi/WiringPi/issues) of GitHub.

If you're not sure whether to create an issue or not, please engage in [discussions](https://github.com/WiringPi/WiringPi/discussions)!

Please do not email Gordon or @Gadgetoid.

Please don't email GC2 for reporting issues, you might [contact us](mailto:wiringpi@gc2.at) for anything that's not meant for the public.

## History

This repository is the continuation of 'Gordon's wiringPi 2.5' which has been [deprecated](https://web.archive.org/web/20220405225008/http://wiringpi.com/wiringpi-deprecated/), a while ago.

* The last "old wiringPi" source of Gordon's release can be found at the
  [`final_source_2.50`](https://github.com/WiringPi/WiringPi/tree/final_official_2.50) tag.
* The default `master` branch contains code that has been written since version 2.5
  to provide support for newer hardware as well as new features.

:information_source:️ Since 2024, [GC2](https://github.com/GrazerComputerClub) has taken over maintenance of the project, supporting new OS versions as well as current hardware generations. We are dedicated to keeping the arguably best-performing GPIO Library for Raspberry Pi running smoothly. We strive to do our best, but please note that this is a community effort, and we cannot provide any guarantees or take responsibility for implementing specific features you may need.

## Debug

WIRINGPI_DEBUG=1 ./my_wiringpi_program

WIRINGPI_DEBUG=1 gpio readall
