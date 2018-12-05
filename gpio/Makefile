#
# Makefile:
#	The gpio command:
#	  A swiss-army knige of GPIO shenanigans.
#	https://projects.drogon.net/wiring-pi
#
#	Copyright (c) 2012-2016 Gordon Henderson
#################################################################################
# This file is part of wiringPi:
#	A "wiring" library for the Raspberry Pi
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

PUBLIC_BIN_DIR				= $(BASE_INSTALL_DIR)bin/
PUBLIC_INCLUDE_DIR			= $(BASE_INSTALL_DIR)include/
PUBLIC_LIB_DIR				= $(BASE_INSTALL_DIR)lib/
PUBLIC_MAN_DIR				= $(BASE_INSTALL_DIR)share/man/man1/

LOCAL_BIN_DIR				= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)bin/
LOCAL_INCLUDE_DIR			= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)include/
LOCAL_LIB_DIR				= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)lib/$(PROJ_INSTALL_DIR)/$(ARCHITECTURE_INSTALL_DIR)
LOCAL_MAN_DIR				= $(BASE_INSTALL_DIR)$(LOCAL_INSTALL_DIR)share/man/man1/

DEB_TEMPLATE_DIR			= ../debian-template/$(PROJECT)/


ifneq ($V,1)
Q ?= @
endif


CC		=	arm-linux-gnueabihf-gcc
#DEBUG	=	-g -O0
DEBUG	=	-O2
INCLUDE	=	-I/$(LOCAL_INCLUDE_DIR)
CFLAGS	=	$(DEBUG) -Wall -Wextra -Werror $(INCLUDE) -Winline -pipe
LDFLAGS	=	-L/$(LOCAL_LIB_DIR)
LIBS    =	-lwiringPi -lwiringPiDev -lpthread -lrt -lm -lcrypt

# May not need to  alter anything below this line
###############################################################################
SOURCES	=	gpio.c readall.c

OBJS	=	$(SOURCES:.c=.o)


all:		gpio

version.h:	../VERSION
	$Q echo Need to run newVersion above.


gpio:	$(OBJS)
	$Q echo [Link]
	$Q $(CC) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)


.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@


.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJS) gpio *~ core tags *.bak


.PHONY:	tags
tags:	$(SOURCES)
	$Q echo [ctags]
	$Q ctags $(SOURCES)


.PHONY:	install
install: gpio
	$Q echo "[Install]"
	$Q if [ "$(shell gcc -dumpmachine)" != "arm-linux-gnueabihf" ]; then \
		echo " *** Cross-build environment, NOT installing binaries ***"; \
	else \
		cp gpio		/$(LOCAL_BIN_DIR); \
		if [ "$(WIRINGPI_SUID)" != "0" ]; then \
				chown root.root	/$(LOCAL_BIN_DIR)gpio; \
				chmod 4755		/$(LOCAL_BIN_DIR)gpio; \
		fi; \
		mkdir -p	/$(LOCAL_MAN_DIR); \
		cp gpio.1	/$(LOCAL_MAN_DIR); \
	fi;


.PHONY:	install-deb
install-deb:	gpio
	$Q echo "[Install: deb]"
	$Q install -m 0755 -d		$(DEB_TEMPLATE_DIR)$(PUBLIC_BIN_DIR)
	$Q install -m 0755 gpio		$(DEB_TEMPLATE_DIR)$(PUBLIC_BIN_DIR)
	
	$Q install -m 0755 -d		$(DEB_TEMPLATE_DIR)$(PUBLIC_MAN_DIR)
	$Q install -m 0644 gpio.1	$(DEB_TEMPLATE_DIR)$(PUBLIC_MAN_DIR)


.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q rm -f /$(LOCAL_BIN_DIR)gpio
	$Q rm -f /$(LOCAL_MAN_DIR)gpio.1


.PHONY:	depend
depend:
	makedepend -Y $(SOURCES)


# DO NOT DELETE

gpio.o: ../version.h
