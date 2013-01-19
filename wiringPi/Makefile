# ;
# Makefile:
#	wiringPi - Wiring Compatable library for the Raspberry Pi
#
#	Copyright (c) 2012 Gordon Henderson
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

DYN_VERS_MAJ=1
DYN_VERS_MIN=0

VERSION=$(DYN_VERS_MAJ).$(DYN_VERS_MIN)
DESTDIR=/usr
PREFIX=/local

STATIC=libwiringPi.a
DYNAMIC=libwiringPi.so.$(VERSION)

#DEBUG	= -g -O0
DEBUG	= -O2
CC	= gcc
INCLUDE	= -I.
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe -fPIC

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	wiringPi.c wiringPiFace.c wiringSerial.c wiringShift.c	\
		gertboard.c						\
		piNes.c							\
		lcd.c piHiPri.c piThread.c				\
		wiringPiSPI.c 						\
		softPwm.c softServo.c softTone.c

SRC_I2C	=	wiringPiI2C.c

OBJ	=	$(SRC:.c=.o)

OBJ_I2C	=	$(SRC_I2C:.c=.o)

all:		$(DYNAMIC)

static:		$(STATIC)

$(STATIC):	$(OBJ)
	@echo "[Link (Static)]"
	@ar rcs $(STATIC) $(OBJ)
	@ranlib $(STATIC)
#	@size   $(STATIC)

$(DYNAMIC):	$(OBJ)
	@echo "[Link (Dynamic)]"
	@$(CC) -shared -Wl,-soname,libwiringPi.so.1 -o libwiringPi.so.1.0 -lpthread $(OBJ)

i2c:	$(OBJ) $(OBJ_I2C)
	@echo "[Link (Dynamic + I2C)]"
	@$(CC) -shared -Wl,-soname,libwiringPi.so.1 -o libwiringPi.so.1.0 -lpthread $(OBJ) $(OBJ_I2C)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPi.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)

.PHONEY:	install
install:	$(DYNAMIC)
	@echo "[Install]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPi.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringSerial.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringShift.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 gertboard.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 piNes.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 softPwm.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 softServo.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 softTone.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 lcd.h			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPiSPI.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPiI2C.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0755 libwiringPi.so.$(VERSION)		  $(DESTDIR)$(PREFIX)/lib
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPi.so.$(VERSION) $(DESTDIR)/lib/libwiringPi.so
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPi.so.$(VERSION) $(DESTDIR)/lib/libwiringPi.so.1
	@ldconfig

.PHONEY:	install-static
install-static:	$(STATIC)
	@echo "[Install Static]"
	@install -m 0755 libwiringPi.a		$(DESTDIR)$(PREFIX)/lib

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPi.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringSerial.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringShift.h
	@rm -f $(DESTDIR)$(PREFIX)/include/gertboard.h
	@rm -f $(DESTDIR)$(PREFIX)/include/piNes.h
	@rm -f $(DESTDIR)$(PREFIX)/include/softPwm.h
	@rm -f $(DESTDIR)$(PREFIX)/include/softServo.h
	@rm -f $(DESTDIR)$(PREFIX)/include/softTone.h
	@rm -f $(DESTDIR)$(PREFIX)/include/lcd.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPiSPI.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPiI2C.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libwiringPi.*
	@ldconfig


.PHONEY:	depend
depend:
	makedepend -Y $(SRC) $(SRC_I2C)

# DO NOT DELETE

wiringPi.o: wiringPi.h
wiringPiFace.o: wiringPi.h
wiringSerial.o: wiringSerial.h
wiringShift.o: wiringPi.h wiringShift.h
gertboard.o: wiringPiSPI.h gertboard.h
piNes.o: wiringPi.h piNes.h
lcd.o: wiringPi.h lcd.h
piHiPri.o: wiringPi.h
piThread.o: wiringPi.h
wiringPiSPI.o: wiringPiSPI.h
softPwm.o: wiringPi.h softPwm.h
softServo.o: wiringPi.h softServo.h
softTone.o: wiringPi.h softTone.h
wiringPiI2C.o: wiringPi.h wiringPiI2C.h
