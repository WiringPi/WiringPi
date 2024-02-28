ℹ️ Since 2024, [GC2](https://github.com/GrazerComputerClub) has taken over maintenance of the project, supporting new OS versions as well as current hardware generations. We are dedicated to keeping the arguably best-performing GPIO Library for Raspberry Pi running smoothly. We strive to do our best, but please note that this is a community effort, and we cannot provide any guarantees or take responsibility for implementing specific features you may need.

⚠️ on Pi5, PWM support is currently under development and _will not work at this point_. If you're interested in the progress, please check the [corresponding issue](https://github.com/GrazerComputerClub/WiringPi/issues/21).
 
WiringPi
=================================

WiringPi is a _performant_ GPIO access library written in C for Raspberry Pi boards.

This repository is the continuation of 'Gordon's wiringPi' which has been [deprecated](https://web.archive.org/web/20220405225008/http://wiringpi.com/wiringpi-deprecated/), a while ago.

  * The last "old wiringPi" source of Gordon's release can be found at the
    [`final_source_2.50`](https://github.com/WiringPi/WiringPi/tree/final_official_2.50) tag.
  * The default `master` branch contains code that has been written since version 2.5
    to provide support for newer hardware as well as new features.

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

Please do not email Gordon or @Gadgetoid.

Please don't email GC2 for reporting issues, you might [contact us](mailto:wiringpi@gc2.at) for anything that's not meant for the public.

Debug
-------

WIRINGPI_DEBUG=1 ./my_wiringpi_program  
WIRINGPI_DEBUG=1 ./gpio readall
