# Makefile:
#	wiringPi device - Wiring Compatable library for the Raspberry Pi
#
#	Copyright (c) 2012-2013 Gordon Henderson
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

DYN_VERS_MAJ=2
DYN_VERS_MIN=0

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC=libwiringPiDev.a
DYNAMIC=libwiringPiDev.so.$(VERSION)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wformat=2 -Wall $(INCLUDE) -Winline -pipe -fPIC

LIBS    =

###############################################################################

SRC	=	ds1302.c maxdetect.c  piNes.c		\
		gertboard.c piFace.c			\
		lcd128x64.c lcd.c			\
		piGlow.c

OBJ	=	$(SRC:.c=.o)

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

.PHONEY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPiDev.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


.PHONEY:	install-headers
install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 ds1302.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 maxdetect.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 piNes.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 gertboard.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 piFace.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 lcd128x64.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 lcd.h			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 piGlow.h		$(DESTDIR)$(PREFIX)/include

.PHONEY:	install
install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiDev.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libwiringPiDev.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPiDev.so.$(VERSION)	$(DESTDIR)/lib/libwiringPiDev.so
	@ldconfig

.PHONEY:	install-static
install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPiDev.a	$(DESTDIR)$(PREFIX)/lib

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/ds1302.h
	@rm -f $(DESTDIR)$(PREFIX)/include/maxdetect.h
	@rm -f $(DESTDIR)$(PREFIX)/include/piNes.h
	@rm -f $(DESTDIR)$(PREFIX)/include/gertboard.h
	@rm -f $(DESTDIR)$(PREFIX)/include/piFace.h
	@rm -f $(DESTDIR)$(PREFIX)/include/lcd128x64.h
	@rm -f $(DESTDIR)$(PREFIX)/include/lcd.h
	@rm -f $(DESTDIR)$(PREFIX)/include/piGlow.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libwiringPiDev.*
	@ldconfig


.PHONEY:	depend
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
