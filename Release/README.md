#Readme

# How to install WiringPi_lib-2.32.0-Linux.deb

  1. you need to remove the preinstall wiringpi package
  2. install my prebuilt *.deb package
  
```bash
      $sudo apt-get remove wiringpi
      $sudo dpkg -i WiringPi_lib-2.32.0-Linux.deb
```
      
  # what the file has been installed to 

     1. libwiringPiDev.so && libwiringPi.so install to /usr/lib/
     2. HEAD_FILES under wiring/ , dev/ install to /usr/local/include
     3. exec binary 'gpio' install to /usr/local/bin/gpio
     
