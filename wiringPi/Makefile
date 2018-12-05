#
# Makefile:
#	wiringPi - Wiring Compatable library for the Raspberry Pi
#
#	Copyright (c) 2012-2015 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	https://projects.drogon.net/raspberry-pi/wiringpi/
#
#    wiringPi is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    wiringPi is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################
PROJECT						=	wiringPi
VERSION						=	$(shell cat ../VERSION)


BASE_INSTALL_DIR			?=	usr/
LOCAL_INSTALL_DIR			?=	local/
PROJ_INSTALL_DIR			?=	$(PROJECT)/
ARCHITECTURE_INSTALL_DIR	?=	armhf/

PUBLIC_INCLUDE_DIR			= $(BASE_INSTALL_DIR)include/
PUBLIC_LIB_DIR				= $(BASE_INSTALL_DIR)lib/
LOCAL_INCLUDE_DIR			= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)include/
LOCAL_LIB_DIR				= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)lib/$(PROJ_INSTALL_DIR)$(ARCHITECTURE_INSTALL_DIR)

DEB_TEMPLATE_DIR			= ../debian-template/$(PROJECT)/


LDCONFIG	?=	ldconfig


ifneq ($V,1)
Q ?= @
endif


STATIC_LIB	=	lib$(PROJECT).a
DYNAMIC_LIB	=	lib$(PROJECT).so


CC		=	arm-linux-gnueabihf-gcc
#DEBUG	=	-g -O0
DEBUG	=	-O2
INCLUDE	=	-I.
DEFS	=	-D_GNU_SOURCE
CFLAGS	=	$(DEBUG) $(DEFS) -Wformat=2 -Wall -Wextra -Winline -Werror $(INCLUDE) -pipe -fPIC
#CFLAGS	=	$(DEBUG) $(DEFS) -Wformat=2 -Wall -Wextra -Wconversion -Winline $(INCLUDE) -pipe -fPIC
LIBS	=	-lm -lpthread -lrt -lcrypt

###############################################################################

SOURCES	=	wiringPi.c \
			wiringSerial.c wiringShift.c \
			piHiPri.c piThread.c \
			wiringPiSPI.c wiringPiI2C.c \
			softPwm.c softTone.c \
			mcp23008.c mcp23016.c mcp23017.c \
			mcp23s08.c mcp23s17.c \
			sr595.c \
			pcf8574.c pcf8591.c \
			mcp3002.c mcp3004.c mcp4802.c mcp3422.c \
			max31855.c max5322.c ads1115.c \
			sn3218.c \
			bmp180.c htu21d.c ds18b20.c rht03.c \
			drcSerial.c drcNet.c \
			pseudoPins.c \
			wpiExtensions.c

HEADERS =	$(shell ls *.h)

OBJS	=	$(SOURCES:.c=.o)


all:		$(DYNAMIC_LIB).$(VERSION)


.PHONY:	static
static:	
	$Q cat noMoreStatic


$(DYNAMIC_LIB).$(VERSION):	$(OBJS)
	$Q echo "[Link (Dynamic)]"
	$Q $(CC) -shared -Wl,-soname,$(DYNAMIC_LIB)$(WIRINGPI_SONAME_SUFFIX) -o $(DYNAMIC_LIB).$(VERSION) $(LIBS) $(OBJS)


.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@


.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJS) $(OBJ_I2C) *~ core tags Makefile.bak lib$(PROJECT).*


.PHONY:	tags
tags:	$(SOURCES)
	$Q echo [ctags]
	$Q ctags $(SOURCES)


.PHONY:	install
install:	$(DYNAMIC_LIB).$(VERSION)
	$Q echo "[Install Headers]"
	$Q install	-m 0644 $(HEADERS)								/$(PUBLIC_INCLUDE_DIR)

	$Q echo "[Install Dynamic Lib]"
	$Q install	-m 0755 -d										/$(LOCAL_LIB_DIR)
	$Q install	-m 0755 $(DYNAMIC_LIB).$(VERSION)				/$(LOCAL_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)
	$Q ln		-sf /$(LOCAL_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)	/$(PUBLIC_LIB_DIR)$(DYNAMIC_LIB)
	$Q $(LDCONFIG)


.PHONY:	install-deb
install-deb:	$(DYNAMIC_LIB).$(VERSION)
	$Q echo "[Install Headers: deb]"
	$Q install -m 0755 -d						$(DEB_TEMPLATE_DIR)$(PUBLIC_INCLUDE_DIR)
	$Q install -m 0644 $(HEADERS)				$(DEB_TEMPLATE_DIR)$(PUBLIC_INCLUDE_DIR)

	$Q echo "[Install Dynamic Lib: deb]"
	install -m 0755 -d							$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)
	install -m 0755 libwiringPi.so.$(VERSION)	$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)
	ln -sf $(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)	$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)$(DYNAMIC_LIB)


.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q cd /$(PUBLIC_INCLUDE_DIR) && rm -f $(HEADERS)
	$Q if [ -d "/$(LOCAL_LIB_DIR)" ]; then cd /$(LOCAL_LIB_DIR) && rm -f $(DYNAMIC_LIB).*; fi;
	$Q cd /$(PUBLIC_LIB_DIR) && rm -f $(DYNAMIC_LIB)
	$Q $(LDCONFIG)


.PHONY:	depend
depend:
	makedepend -Y $(SOURCES) $(SRC_I2C)

# DO NOT DELETE

wiringPi.o: softPwm.h softTone.h wiringPi.h ../version.h
wiringSerial.o: wiringSerial.h
wiringShift.o: wiringPi.h wiringShift.h
piHiPri.o: wiringPi.h
piThread.o: wiringPi.h
wiringPiSPI.o: wiringPi.h wiringPiSPI.h
wiringPiI2C.o: wiringPi.h wiringPiI2C.h
softPwm.o: wiringPi.h softPwm.h
softTone.o: wiringPi.h softTone.h
mcp23008.o: wiringPi.h wiringPiI2C.h mcp23x0817.h mcp23008.h
mcp23016.o: wiringPi.h wiringPiI2C.h mcp23016.h mcp23016reg.h
mcp23017.o: wiringPi.h wiringPiI2C.h mcp23x0817.h mcp23017.h
mcp23s08.o: wiringPi.h wiringPiSPI.h mcp23x0817.h mcp23s08.h
mcp23s17.o: wiringPi.h wiringPiSPI.h mcp23x0817.h mcp23s17.h
sr595.o: wiringPi.h sr595.h
pcf8574.o: wiringPi.h wiringPiI2C.h pcf8574.h
pcf8591.o: wiringPi.h wiringPiI2C.h pcf8591.h
mcp3002.o: wiringPi.h wiringPiSPI.h mcp3002.h
mcp3004.o: wiringPi.h wiringPiSPI.h mcp3004.h
mcp4802.o: wiringPi.h wiringPiSPI.h mcp4802.h
mcp3422.o: wiringPi.h wiringPiI2C.h mcp3422.h
max31855.o: wiringPi.h wiringPiSPI.h max31855.h
max5322.o: wiringPi.h wiringPiSPI.h max5322.h
ads1115.o: wiringPi.h wiringPiI2C.h ads1115.h
sn3218.o: wiringPi.h wiringPiI2C.h sn3218.h
bmp180.o: wiringPi.h wiringPiI2C.h bmp180.h
htu21d.o: wiringPi.h wiringPiI2C.h htu21d.h
ds18b20.o: wiringPi.h ds18b20.h
drcSerial.o: wiringPi.h wiringSerial.h drcSerial.h
pseudoPins.o: wiringPi.h pseudoPins.h
wpiExtensions.o: wiringPi.h mcp23008.h mcp23016.h mcp23017.h mcp23s08.h
wpiExtensions.o: mcp23s17.h sr595.h pcf8574.h pcf8591.h mcp3002.h mcp3004.h
wpiExtensions.o: mcp4802.h mcp3422.h max31855.h max5322.h ads1115.h sn3218.h
wpiExtensions.o: drcSerial.h pseudoPins.h bmp180.h htu21d.h ds18b20.h
wpiExtensions.o: wpiExtensions.h
