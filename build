#!/bin/bash

check-make-ok()
{
  if [ $? != 0 ]; then
    echo ""
    echo "Make Failed..."
    echo "Please check the messages and fix any problems. If you're still stuck,"
    echo "then please email all the output and as many details as you can to"
    echo "  projects@drogon.net"
    echo ""
    exit 1
  fi
}

if [ x$1 = "xclean" ]; then
  echo Cleaning
  echo
  cd wiringPi
  make clean
  cd ../gpio
  make clean
  cd ../examples
  make clean
  exit
fi

if [ x$1 = "xuninstall" ]; then
  echo Uninstalling
  echo
  echo "WiringPi library"
  cd wiringPi
  sudo make uninstall
  echo
  echo "GPIO Utility"
  cd ../gpio
  sudo make uninstall
  cd ..
  exit
fi


  echo "wiringPi Build script"
  echo "====================="
  echo

# Check for I2C being installed...
#	... and if-so, then automatically make the I2C helpers

  if [ -f /usr/include/linux/i2c-dev.h ]; then
    grep -q i2c_smbus_read_byte /usr/include/linux/i2c-dev.h
    if [ $? = 0 ]; then
      target=i2c
      echo "Building wiringPi with the I2C helper libraries."
    else
      target=all
      echo "The wiringPi I2C helper libraries will not be built."
    fi
  fi

  echo
  echo "WiringPi library"
  cd wiringPi
  sudo make uninstall
  make $target
  check-make-ok
  sudo make install
  check-make-ok

  echo
  echo "GPIO Utility"
  cd ../gpio
  make
  check-make-ok
  sudo make install
  check-make-ok

# echo
# echo "Examples"
# cd ../examples
# make
# cd ..

echo
echo All Done.
