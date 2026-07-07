## Quick Start

g++ is required to build the test cases.

Connect wPi pin 1 (BCM 18) and wPi pin 0 (BCM 17) to run ISR tests.
Also, wPi pin 6 (BCM 25) should be unconnected.

Run `make test` or do following to run the test:

```
make googletest-main
make -j 4
make test
```

To clean up the environment:

```
make clean
```

## Dependencies

Googletest is downloaded during
the build for the first time. The downloaded items are in directory `googletest-main`. `make clean` keeps this directory.  Run `make distclean`
to erase all built images including googletest.
