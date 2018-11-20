#
# Makefile:
#	wiringPi device - A "wiring" library for the Raspberry Pi
#
#	Copyright (c) 2012-2016 Gordon Henderson
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

PUBLIC_INCLUDE_DIR			= /$(BASE_INSTALL_DIR)include/
PUBLIC_LIB_DIR				= /$(BASE_INSTALL_DIR)lib/
LOCAL_INCLUDE_DIR			= /$(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)include/
LOCAL_LIB_DIR				= /$(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)lib/$(PROJ_INSTALL_DIR)/$(ARCHITECTURE_INSTALL_DIR)

DEB_TEMPLATE_DIR			= ../debian-template/$(PROJECT)/


LDCONFIG	?=	ldconfig


ifneq ($V,1)
Q ?= @
endif


STATIC_LIB	=	lib$(PROJECT)Dev.a
DYNAMIC_LIB	=	lib$(PROJECT)Dev.so


CC		=	arm-linux-gnueabihf-gcc
#DEBUG	=	-g -O0
DEBUG	=	-O2
INCLUDE	=	-I.
DEFS	=	-D_GNU_SOURCE
CFLAGS	=	$(DEBUG) $(DEFS) -Wformat=2 -Wall -Winline $(INCLUDE) -pipe -fPIC
LIBS    =

###############################################################################

SOURCES	=	ds1302.c maxdetect.c piNes.c \
			gertboard.c piFace.c \
			lcd128x64.c lcd.c \
			scrollPhat.c \
			piGlow.c

OBJS	=	$(SOURCES:.c=.o)

HEADERS	=	ds1302.h gertboard.h lcd128x64.h lcd.h maxdetect.h piFace.h piGlow.h piNes.h \
			scrollPhat.h


all:		$(DYNAMIC_LIB).$(VERSION)


static:		$(STATIC_LIB)


$(STATIC_LIB):	$(OBJS)
	$Q echo "[Link (Static)]"
	$Q ar rcs $(STATIC_LIB) $(OBJS)
	$Q ranlib $(STATIC_LIB)
#	@size   $(STATIC_LIB)


$(DYNAMIC_LIB).$(VERSION):	$(OBJS)
	$Q echo "[Link (Dynamic)]"
	$Q $(CC) -shared -Wl,-soname,$(DYNAMIC_LIB)$(WIRINGPI_SONAME_SUFFIX) -o $(DYNAMIC_LIB).$(VERSION) -lpthread $(OBJS)


.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@


.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJS) $(OBJ_I2C) *~ core tags Makefile.bak lib$(PROJECT)Dev*.*


.PHONY:	tags
tags:	$(SRC)
	$Q echo [ctags]
	$Q ctags $(SOURCES)


.PHONY:	install
install:	$(DYNAMIC_LIB).$(VERSION)
	$Q echo "[Install Headers]"
	$Q install	-m 0755 -d										$(LOCAL_INCLUDE_DIR)
	$Q install	-m 0644 $(HEADERS)								$(LOCAL_INCLUDE_DIR)

	$Q echo "[Install Dynamic Lib]"
	$Q install	-m 0755 -d										$(LOCAL_LIB_DIR)
	$Q install	-m 0755 $(DYNAMIC_LIB).$(VERSION)				$(LOCAL_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)
	$Q ln 		-sf $(LOCAL_LIB_DIR)$(DYNAMIC_LIB).$(VERSION)	$(PUBLIC_LIB_DIR)$(DYNAMIC_LIB)
	$Q $(LDCONFIG)


.PHONY:	install-static
install-static:	$(STATIC)
	$Q echo "[Install Headers]"
	$Q install	-m 0755 -d				$(LOCAL_INCLUDE_DIR)
	$Q install	-m 0644 $(HEADERS)		$(LOCAL_INCLUDE_DIR)

	$Q echo "[Install Static Lib]"
	$Q install	-m 0755 -d				$(LOCAL_LIB_DIR)
	$Q install	-m 0755 $(STATIC_LIB)	$(LOCAL_LIB_DIR)


.PHONY:	install-deb
install-deb:	$(DYNAMIC)
	$Q echo "[Install Headers: deb]"
	$Q install -m 0755 -d						$(DEB_TEMPLATE_DIR)$(PUBLIC_INCLUDE_DIR)
	$Q install -m 0644 $(HEADERS)				$(DEB_TEMPLATE_DIR)$(PUBLIC_INCLUDE_DIR)

	$Q echo "[Install Dynamic Lib: deb]"
	install -m 0755 -d							$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)
	install -m 0755 $(DYNAMIC_LIB).$(VERSION)	$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)/$(DYNAMIC_LIB).$(VERSION)
	ln -sf $(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)/$(DYNAMIC_LIB).$(VERSION)	$(DEB_TEMPLATE_DIR)$(PUBLIC_LIB_DIR)/$(DYNAMIC_LIB)


.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q cd $(LOCAL_INCLUDE_DIR) && rm -f $(HEADERS)
	$Q if [ -d "$(LOCAL_LIB_DIR)" ]; then cd $(LOCAL_LIB_DIR) && rm -f $(DYNAMIC_LIB).* && rm -f $(STATIC_LIB).*; fi
	$Q cd $(PUBLIC_LIB_DIR) && rm -f $(DYNAMIC_LIB)
	$Q $(LDCONFIG)


.PHONY:	depend
depend:
	makedepend -Y $(SOURCES)


# DO NOT DELETE

ds1302.o: ds1302.h
maxdetect.o: maxdetect.h
piNes.o: piNes.h
gertboard.o: gertboard.h
piFace.o: piFace.h
lcd128x64.o: font.h lcd128x64.h
lcd.o: lcd.h
scrollPhat.o: scrollPhatFont.h scrollPhat.h
piGlow.o: piGlow.h
