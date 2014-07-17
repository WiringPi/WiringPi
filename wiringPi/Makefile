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

DYN_VERS_MAJ=2
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
DEFS	= -D_GNU_SOURCE
CFLAGS	= $(DEBUG) $(DEFS) -Wformat=2 -Wall -Winline $(INCLUDE) -pipe -fPIC

LIBS    =

# Should not alter anything below this line
###############################################################################

SRC	=	wiringPi.c						\
		wiringSerial.c wiringShift.c				\
		piHiPri.c piThread.c					\
		wiringPiSPI.c wiringPiI2C.c				\
		softPwm.c softTone.c					\
		mcp23008.c mcp23016.c mcp23017.c			\
		mcp23s08.c mcp23s17.c					\
		sr595.c							\
		pcf8574.c pcf8591.c					\
		mcp3002.c mcp3004.c mcp4802.c mcp3422.c			\
		max31855.c max5322.c					\
		sn3218.c						\
		drcSerial.c

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
	@$(CC) -shared -Wl,-soname,libwiringPi.so -o libwiringPi.so.$(VERSION) -lpthread $(OBJ)

.c.o:
	@echo [Compile] $<
	@$(CC) -c $(CFLAGS) $< -o $@

.PHONEY:	clean
clean:
	@echo "[Clean]"
	@rm -f $(OBJ) $(OBJ_I2C) *~ core tags Makefile.bak libwiringPi.*

.PHONEY:	tags
tags:	$(SRC)
	@echo [ctags]
	@ctags $(SRC)


.PHONEY:	install-headers
install-headers:
	@echo "[Install Headers]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPi.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringSerial.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringShift.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 softPwm.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 softTone.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPiSPI.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 wiringPiI2C.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 drcSerial.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23008.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23016.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23017.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23s08.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp23s17.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 max31855.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 max5322.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp3002.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp3004.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp4802.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 mcp3422.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 sr595.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 pcf8574.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 pcf8591.h		$(DESTDIR)$(PREFIX)/include
	@install -m 0644 sn3218.h		$(DESTDIR)$(PREFIX)/include

.PHONEY:	install
install:	$(DYNAMIC) install-headers
	@echo "[Install Dynamic Lib]"
	@install -m 0755 -d						$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPi.so.$(VERSION)			$(DESTDIR)$(PREFIX)/lib/libwiringPi.so.$(VERSION)
	@ln -sf $(DESTDIR)$(PREFIX)/lib/libwiringPi.so.$(VERSION)	$(DESTDIR)/lib/libwiringPi.so
	@ldconfig

.PHONEY:	install-static
install-static:	$(STATIC) install-headers
	@echo "[Install Static Lib]"
	@install -m 0755 -d			$(DESTDIR)$(PREFIX)/lib
	@install -m 0755 libwiringPi.a		$(DESTDIR)$(PREFIX)/lib

.PHONEY:	uninstall
uninstall:
	@echo "[UnInstall]"
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPi.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringSerial.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringShift.h
	@rm -f $(DESTDIR)$(PREFIX)/include/softPwm.h
	@rm -f $(DESTDIR)$(PREFIX)/include/softTone.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPiSPI.h
	@rm -f $(DESTDIR)$(PREFIX)/include/wiringPiI2C.h
	@rm -f $(DESTDIR)$(PREFIX)/include/drcSerial.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23008.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23016.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23017.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23s08.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp23s17.h
	@rm -f $(DESTDIR)$(PREFIX)/include/max31855.h
	@rm -f $(DESTDIR)$(PREFIX)/include/max5322.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp3002.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp3004.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp4802.h
	@rm -f $(DESTDIR)$(PREFIX)/include/mcp3422.h
	@rm -f $(DESTDIR)$(PREFIX)/include/sr595.h
	@rm -f $(DESTDIR)$(PREFIX)/include/pcf8574.h
	@rm -f $(DESTDIR)$(PREFIX)/include/pcf8591.h
	@rm -f $(DESTDIR)$(PREFIX)/include/sn3218.h
	@rm -f $(DESTDIR)$(PREFIX)/lib/libwiringPi.*
	@ldconfig


.PHONEY:	depend
depend:
	makedepend -Y $(SRC) $(SRC_I2C)

# DO NOT DELETE

wiringPi.o: softPwm.h softTone.h wiringPi.h
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
sn3218.o: wiringPi.h wiringPiI2C.h sn3218.h
drcSerial.o: wiringPi.h wiringSerial.h drcSerial.h
