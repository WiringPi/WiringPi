
:warning: This library was deprecated by its author in August 2019. Until 31st October 2023 it was maintaining at https://github.com/WiringPi/WiringPi/ but later set to read only because there was no interest in properly maintaining it.
In 2024 GC2 fork the project to maintaining it and to keep the best GPIO Library for Raspberry Pi running. We do our best, but we have limited resources and can not give support. 

WiringPi (Unofficial Mirror/Fork)
=================================

This is an unofficial mirror/fork of wiringPi to support ports (Python/Ruby/etc).  With the
[end of official development](https://web.archive.org/web/20220405225008/http://wiringpi.com/wiringpi-deprecated/), this repository
has become a mirror of the last "official" source release, plus a fork facilitating updates
to support newer hardware (primarily for use by the ports) and fix bugs.

  * The final "official" source release can be found at the
    [`final_source_2.50`](https://github.com/WiringPi/WiringPi/tree/final_official_2.50) tag.
  * The default `master` branch contains code that has been written since that final source
    release to provide support for newer hardware.

Raspberry Pi 5 Roadmap
-----

* 2.71 Raspberry Pi 4 (Bookworm)
* 2.72 Raspberry Pi 5 sysfs support
* 2.73 Raspberry Pi 5 gpiomem support

BananaPi M2 Zero 
-----

See brunch BananPi, there is no further development.


Ports
-----

wiringPi has been wrapped for multiple languages:

* Node - https://github.com/WiringPi/WiringPi-Node
* Perl - https://github.com/WiringPi/WiringPi-Perl
* PHP - https://github.com/WiringPi/WiringPi-PHP
* Python - https://github.com/WiringPi/WiringPi-Python
* Ruby - https://github.com/WiringPi/WiringPi-Ruby

Support
-------

Please do not email Gordon if you have issues, he will not be able to help.  
Dont' email @Gadgetoid.  
Don't email GC2, use issue system of github to report errors.

Debug
-------

WIRINGPI_DEBUG=1 ./my_wiringpi_program  
WIRINGPI_DEBUG=1 ./gpio readall
