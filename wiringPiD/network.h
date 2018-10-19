/*
 * network.h:
 *	Part of wiringPiD
 *	Copyright (c) 2012-2017 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

extern char *getClientIP   (void) ;
extern int   getResponce   (int clientFd) ;
extern int   setupServer   (int serverPort) ;
extern int   sendGreeting  (int clientFd) ;
extern int   sendChallenge (int clientFd) ;
extern int   getResponse   (int clientFd) ;
extern int   passwordMatch (const char *password) ;
extern void  closeServer   (int clientFd) ;
