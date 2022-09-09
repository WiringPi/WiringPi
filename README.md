WiringPi (Unofficial Mirror/Fork)
=================================

This is an unofficial mirror/fork of wiringPi to support ports (Python/Ruby/etc).  With the
[end of official development](http://wiringpi.com/wiringpi-deprecated/), this repository
has become a mirror of the last "official" source release, plus a fork facilitating updates
to support newer hardware (primarily for use by the ports) and fix bugs.

  * The final "official" source release can be found at the
    [`final_source_2.50`](https://github.com/WiringPi/WiringPi/tree/final_official_2.50) tag.
  * The default `master` branch contains code that has been written since that final source
    release to provide support for newer hardware.

Build
-----

```shell
mdkir dist
cd dist
cmake ..
cmake --build .
```

*Note: You can also cross-compile using the `toolchain-rpi.cmake` file available in the `cmake/` directory. Run cmake with -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-rpi.cmake*

Debian package:
```shell
cpack -config CPackConfig.cmake
```

Build Archive:
```shell
cpack -config CPackSourceConfig.cmake
```

Install
-------

To install the wiringPi library, simply download the `.deb` package available in the [`latest release`](https://github.com/WiringPi/WiringPi/releases/latest) or build it from source and install it using `dpkg`:
```shell
sudo dpkg -i wiringpi-2.61-1-armhf.deb
```

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

Pull-requests may be accepted to add or fix support for newer hardware, but new features or
other changes may not be accepted.

For support, comments, questions, etc please join the WiringPi Discord channel: https://discord.gg/SM4WUVG
