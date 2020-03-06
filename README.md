Unofficial WiringPi Mirror
==========================

This is an unofficial mirror of WiringPi to support ports (Python/Ruby/etc).  With the
[end of official development](http://wiringpi.com/wiringpi-deprecated/), this repository
has become a mirror of the last "official" source release as well as a source for small
updates to support newer hardware (primarily for use by the ports).

  * The final "official" source release can be found at the
    [`final_source_2.50`](https://github.com/WiringPi/WiringPi/tree/final_official_2.50) tag.
  * The default `master` branch contains code that has been written since that final source
    release to provide support for newer hardware.

Please do not email Gordon if you have issues, he will not be able to help.

Pull-requests may be accepted to add or fix support for newer hardware, but new features or
other changes will not be accepted.

For support, comments, questions, etc please join the WiringPi Discord channel: https://discord.gg/SM4WUVG

## CMake
You can use CMake to cross compile WiringPi. This is heavily inspired by Alex C. U.'s article [The Useful RaspberryPi Cross Compile Guide](https://medium.com/@au42/the-useful-raspberrypi-cross-compile-guide-ea56054de187).

The main difference is that it assumes you are using [Embedded Development Infrastructure (edi)](https://docs.get-edi.io/en/latest/introduction.html). For more info, follow Matthias Lüscher’s [Cross Compiling for Raspbian Buster](https://www.get-edi.io/Cross-Compiling-for-Raspbian-Buster/) guide to set up an edi container.

Do the following steps in a local clone of this repository inside a `raspbian-buster-cross` lxd container:
```
$ mkdir build_cmake
$ cd build_cmake
$ cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/rpi.cmake
$ make
$ sudo make install
```
