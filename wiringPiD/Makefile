#
# Makefile:
#	The wiringPiD utility:
#	https://github.com/wiringPi/wiringPi
#
#	Copyright (c) 2012-2017 Gordon Henderson
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

DESTDIR?=/usr
PREFIX?=/local

ifneq ($V,1)
Q ?= @
endif

#DEBUG	= -g -O0
DEBUG	= -O2
CC	?= gcc
INCLUDE	= -I$(DESTDIR)$(PREFIX)/include
CFLAGS	= $(DEBUG) -Wall -Wextra $(INCLUDE) -Winline -pipe $(EXTRA_CFLAGS)

LDFLAGS	= -L$(DESTDIR)$(PREFIX)/lib
LIBS    = -lwiringPi -lwiringPiDev -lpthread -lrt -lm -lcrypt

# May not need to  alter anything below this line
###############################################################################

SRC	=	wiringpid.c network.c runRemote.c daemonise.c

OBJ	=	$(SRC:.c=.o)

all:		wiringpid

wiringpid:	$(OBJ)
	$Q echo [Link]
	$Q $(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

.c.o:
	$Q echo [Compile] $<
	$Q $(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	$Q echo "[Clean]"
	$Q rm -f $(OBJ) wiringpid *~ core tags *.bak

.PHONY:	tags
tags:	$(SRC)
	$Q echo [ctags]
	$Q ctags $(SRC)

.PHONY:	install
install: wiringpid
	$Q echo "[Install]"
	$Q mkdir -p		$(DESTDIR)$(PREFIX)/sbin
	$Q cp wiringpid		$(DESTDIR)$(PREFIX)/sbin
	$Q chown root.root	$(DESTDIR)$(PREFIX)/sbin/wiringpid

#	$Q mkdir -p		$(DESTDIR)$(PREFIX)/man/man8
#	$Q cp gpio.1		$(DESTDIR)$(PREFIX)/man/man8

.PHONY:	install-deb
install-deb:	gpio
	$Q echo "[Install: deb]"
	$Q install -m 0755 -d							$(DEB_DESTDIR)/usr/bin
	$Q install -m 0755 gpio							$(DEB_DESTDIR)/usr/bin
	$Q install -m 0755 -d							$(DEB_DESTDIR)/man/man1
	$Q install -m 0644 gpio.1						$(DEB_DESTDIR)/man/man1

.PHONY:	uninstall
uninstall:
	$Q echo "[UnInstall]"
	$Q rm -f $(DESTDIR)$(PREFIX)/sbin/wiringpid
	$Q rm -f $(DESTDIR)$(PREFIX)/man/man8/wiringpid.8

.PHONY:	depend
depend:
	makedepend -Y $(SRC)
# DO NOT DELETE

wiringpid.o: drcNetCmd.h network.h runRemote.h daemonise.h
network.o: network.h
runRemote.o: drcNetCmd.h network.h runRemote.h
daemonise.o: daemonise.h
