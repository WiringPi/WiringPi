#
# Makefile:
#	wiringPi device - Wiring Compatable library for the Raspberry Pi
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

VERSION=$(shell cat ../VERSION)
DESTDIR=/usr
PREFIX=/local

STATIC=libwiringPiDev.a
DYNAMIC=libwiringPiDev.so.$(VERSION)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEBUG) $(DEFS) -Wformat=2 -Wall -Winline $(INCLUDE) -pipe -fPIC

LIBS    =

###############################################################################

SRC	=	ds1302.c maxdetect.c  piNes.c		\
		gertboard.c piFace.c			\
		lcd128x64.c lcd.c			\
		piGlow.c

OBJ	=	$(SRC:.c=.o)

HEADERS	=	ds1302.h gertboard.h  lcd128x64.h  lcd.h  maxdetect.h piFace.h  piGlow.h  piNes.h

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libwiringPiDev.so -o libwiringPiDev.so.$(VERSION) -lpthread $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPiDev.*

.PHONY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


.PHONY:	install
install:	$(DYNAMIC)
	@echo "[Install Headers]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/include
	@install -m 0644 $(HEADERS)					$(DESTDIR)$(PREFIX)/include
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiDev.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libwiringPiDev.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPiDev.so.$(VERSION)	$(DESTDIR)/lib/libwiringPiDev.so
	@ldconfig

.PHONY:	install-static
install-static:	$(STATIC)
	@echo "[Install Headers]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/include
	@install -m 0644 $(HEADERS)					$(DESTDIR)$(PREFIX)/include
	@echo "[Install Static Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiDev.a				$(DESTDIR)$(PREFIX)/lib

.PHONY:	install-deb
install-deb:	$(DYNAMIC)
	@echo "[Install Headers: deb]"
	@install -m 0755 -d							~/wiringPi/debian/wiringPi/usr/include
	@install -m 0644 $(HEADERS)						~/wiringPi/debian/wiringPi/usr/include
	@echo "[Install Dynamic Lib: deb]"
	install -m 0755 -d							~/wiringPi/debian/wiringPi/usr/lib
	install -m 0755 libwiringPiDev.so.$(VERSION)				~/wiringPi/debian/wiringPi/usr/lib/libwiringPiDev.so.$(VERSION)
	ln -sf ~/wiringPi/debian/wiringPi/usr/lib/libwiringPi.so.$(VERSION)	~/wiringPi/debian/wiringPi/usr/lib/libwiringPiDev.so

.PHONY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@cd $(DESTDIR)$(PREFIX)/include/ && rm -f $(HEADERS)
	@cd $(DESTDIR)$(PREFIX)/lib/     && rm -f libwiringPiDev.*
	@ldconfig


.PHONY:	depend
depend:
	makedepend -Y $(SRC)

# DO NOT DELETE

ds1302.o: ds1302.h
maxdetect.o: maxdetect.h
piNes.o: piNes.h
gertboard.o: gertboard.h
piFace.o: piFace.h
lcd128x64.o: font.h lcd128x64.h
lcd.o: lcd.h
piGlow.o: piGlow.h
