ℹ️ Since 2024, [GC2](https://github.com/GrazerComputerClub) has taken over maintenance of the project, supporting new OS versions as well as current hardware generations. We are dedicated to keeping the arguably best-performing GPIO Library for Raspberry Pi running smoothly. We strive to do our best, but please note that this is a community effort, and we cannot provide any guarantees or take responsibility for implementing specific features you may need.

⚠️ on Pi5, PWM support is currently under development and _will not work at this point_. If you're interested in the progress, please check the [corresponding issue](https://github.com/GrazerComputerClub/WiringPi/issues/21).
 
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

Please use the [issue system](https://github.com/WiringPi/WiringPi/issues) of GitHub.
Please do not email Gordon if you have issues.
Dont' email @Gadgetoid.
Please don't email GC2 for reporting issues, you might [contact us](wiringpi@gc2.at) for anything that's not meant for the public.

Debug
-------

WIRINGPI_DEBUG=1 ./my_wiringpi_program  
WIRINGPI_DEBUG=1 ./gpio readall
