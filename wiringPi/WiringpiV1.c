#include "Wiringpi.h"


int requestLineV1(int pin, unsigned int lineRequestFlags) {
    struct gpiohandle_request rq;
  
     if (lineFds[pin]>=0) {
      if (lineRequestFlags == lineFlags[pin]) {
        //already requested
        return lineFds[pin];
      } else {
        //different request -> rerequest
        releaseLine(pin);
      }
    }
  
    //requested line
    if (wiringPiGpioDeviceGetFd()<0) {
      return -1;  // error
    }
    rq.lineoffsets[0] = pin;
    rq.lines = 1;
    rq.flags = 0; 
    // MAP to V1 Flag 
    if (lineRequestFlags & WPI_FLAG_INPUT) {
      rq.flags |= GPIOHANDLE_REQUEST_INPUT;
    }
    if (lineRequestFlags & WPI_FLAG_OUTPUT) {
      rq.flags |= GPIOHANDLE_REQUEST_OUTPUT;
    }
    if (lineRequestFlags & WPI_FLAG_BIAS_OFF) {
      rq.flags |= GPIOHANDLE_REQUEST_BIAS_DISABLE;
    }
    if (lineRequestFlags & WPI_FLAG_BIAS_UP) {
      rq.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_UP;
    }
    if (lineRequestFlags & WPI_FLAG_BIAS_DOWN) {
      rq.flags |= GPIOHANDLE_REQUEST_BIAS_PULL_DOWN;
    }
    int ret = ioctl(chipFd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
    if (ret || rq.fd<0) {
      ReportDeviceError("get line handle", pin, "RequestLineV1", ret);
      return -1;  // error
    }
  
    lineFlags[pin] = lineRequestFlags;
    lineFds[pin] = rq.fd;
    if (wiringPiDebug)
      printf ("requestLine succeeded: pin:%d, flags: %u, fd :%d\n", pin, lineRequestFlags, lineFds[pin]) ;
    return lineFds[pin];
  }
  

  int digitalReadDeviceV1(int pin) {   // INPUT and OUTPUT should work

    if (lineFds[pin]<0) {
     // line not requested - auto request on first read as input
     pinModeDevice(pin, INPUT);
   }
   if (lineFds[pin]>=0) {
     struct gpiohandle_data data;
     int ret = ioctl(lineFds[pin], GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
     if (ret) {
       ReportDeviceError("get line values", pin, "digitalRead", ret);
       return LOW;  // error
     }
     return data.values[0];
   }
   return LOW;  // error , need to request line before
 }


 void digitalWriteDeviceV1(int pin, int value) {

    if (wiringPiDebug)
      printf ("digitalWriteDevice: ioctl pin:%d value: %d\n", pin, value) ;
  
    if (lineFds[pin]<0) {
      // line not requested - auto request on first write as output
      pinModeDevice(pin, OUTPUT);
    }
    if (lineFds[pin]>=0 && (lineFlags[pin] & GPIOHANDLE_REQUEST_OUTPUT)>0) {
      struct gpiohandle_data data;
      data.values[0] = value;
      if (wiringPiDebug)
        printf ("digitalWriteDevice: ioctl pin:%d cmd: GPIOHANDLE_SET_LINE_VALUES_IOCTL, value: %d\n", pin, value) ;
      int ret = ioctl(lineFds[pin], GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
      if (ret) {
        ReportDeviceError("set line values", pin, "digitalWrite", ret);
        return;  // error
      }
    } else {
      fprintf(stderr, "digitalWrite: no output (%d)\n", lineFlags[pin]);
    }
    return; // error
  }

 /*
 * waitForInterrupt:
 *	Pi Specific.
 *	Wait for Interrupt on a GPIO pin.
 *	This is actually done via the /dev/gpiochip interface regardless of
 *	the wiringPi access mode in-use. Maybe sometime it might get a better
 *	way for a bit more efficiency.
 *********************************************************************************
 */

int waitForInterruptV1(int pin, int mS)
{
  int fd, ret;
  struct pollfd polls ;
  struct gpioevent_data evdata;
  //struct gpio_v2_line_request req2;

  if (wiringPiMode == WPI_MODE_PINS)
    pin = pinToGpio [pin] ;
  else if (wiringPiMode == WPI_MODE_PHYS)
    pin = physToGpio [pin] ;

  if ((fd = isrFds [pin]) == -1)
    return -2 ;

  // Setup poll structure
  polls.fd      = fd;
  polls.events  = POLLIN | POLLERR ;
  polls.revents = 0;

  // Wait for it ...
  ret = poll(&polls, 1, mS);
  if (ret <= 0) {
    fprintf(stderr, "wiringPi: ERROR: poll returned=%d\n", ret);
  } else {
    //if (polls.revents & POLLIN)
    if (wiringPiDebug) {
      printf ("wiringPi: IRQ line %d received %d, fd=%d\n", pin, ret, isrFds[pin]) ;
    }
    /* read event data */
    int readret = read(isrFds [pin], &evdata, sizeof(evdata));
    if (readret == sizeof(evdata)) {
      if (wiringPiDebug) {
        printf ("wiringPi: IRQ data id: %d, timestamp: %lld\n", evdata.id, evdata.timestamp) ;
      }
      ret = evdata.id;
    } else {
      ret = 0;
    }
  }
  return ret;
}

int waitForInterruptInitV1(int pin, int mode)
{
  const char* strmode = "";

  if (wiringPiMode == WPI_MODE_PINS) {
    pin = pinToGpio [pin] ;
  } else if (wiringPiMode == WPI_MODE_PHYS) {
    pin = physToGpio [pin] ;
  }

  /* open gpio */
  sleep(1);
  if (wiringPiGpioDeviceGetFd()<0) {
    return -1;
  }

  struct gpioevent_request req;
  req.lineoffset = pin;
  req.handleflags = GPIOHANDLE_REQUEST_INPUT;
  switch(mode) {
    default:
    case INT_EDGE_SETUP:
      if (wiringPiDebug) {
        printf ("wiringPi: waitForInterruptMode mode INT_EDGE_SETUP - exiting\n") ;
      }
      return -1;
    case INT_EDGE_FALLING:
      req.eventflags  = GPIOEVENT_REQUEST_FALLING_EDGE;
      strmode = "falling";
      break;
    case INT_EDGE_RISING:
      req.eventflags  = GPIOEVENT_REQUEST_RISING_EDGE;
      strmode = "rising";
      break;
    case INT_EDGE_BOTH:
      req.eventflags  = GPIOEVENT_REQUEST_BOTH_EDGES;
      strmode = "both";
      break;
  }
  strncpy(req.consumer_label, "wiringpi_gpio_irq", sizeof(req.consumer_label) - 1);

  //later implement GPIO_V2_GET_LINE_IOCTL req2
  int ret = ioctl(chipFd, GPIO_GET_LINEEVENT_IOCTL, &req);
  if (ret) {
    ReportDeviceError("get line event", pin , strmode, ret);
    return -1;
  }
  if (wiringPiDebug) {
    printf ("wiringPi: GPIO get line %d , mode %s succeded, fd=%d\n", pin, strmode, req.fd) ;
  }

  /* set event fd nonbloack read */
  int fd_line = req.fd;
  isrFds [pin] = fd_line;
  int flags = fcntl(fd_line, F_GETFL);
  flags |= O_NONBLOCK;
  ret = fcntl(fd_line, F_SETFL, flags);
  if (ret) {
    fprintf(stderr, "wiringPi: ERROR: fcntl set nonblock return=%d\n", ret);
    return -1;
  }

  return 0;
}


int waitForInterruptClose (int pin) {
  if (isrFds[pin]>0) {
    if (wiringPiDebug) {
      printf ("wiringPi: waitForInterruptClose close thread 0x%lX\n", (unsigned long)isrThreads[pin]) ;
    }
    if (pthread_cancel(isrThreads[pin]) == 0) {
      if (wiringPiDebug) {
        printf ("wiringPi: waitForInterruptClose thread canceled successfuly\n") ;
      }
    } else {
     if (wiringPiDebug) {
        fprintf (stderr, "wiringPi: waitForInterruptClose could not cancel thread\n");
      }
    }
    close(isrFds [pin]);
  }
  isrFds [pin] = -1;
  isrFunctions [pin] = NULL;

  /* -not closing so far - other isr may be using it - only close if no other is using - will code later
  if (chipFd>0) {
    close(chipFd);
  }
  chipFd = -1;
  */
  if (wiringPiDebug) {
    printf ("wiringPi: waitForInterruptClose finished\n") ;
  }
  return 0;
}
