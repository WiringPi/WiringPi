/*
 * piHiPri:
 *  Simple way to get your program running at high priority
 *  with realtime schedulling.
 *
 *  Copyright (c) 2012 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *  https://github.com/WiringPi/WiringPi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with wiringPi.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <sched.h>
#include <string.h>

#include "wiringPi.h"

/*
 * piHiPri:
 *  Attempt to set a high priority schedulling for the running program.
 *  A lower priority number = higher priority in the system.
 *  On linux, this range is 1 through 99.
 *  Returns result of sched_setscheduler() call: 0 on success, -1 on error.
 *********************************************************************************
 * @NOTE: This could be done using "int nice(int inc);" in <unistd.h>
 *        nice() adds inc to the nice value for the calling thread.
 *        A higher inc value means a lower priority.
 *        A negative inc value means higher priority.
 *********************************************************************************
 */
int piHiPri (const int pri)
{
  struct sched_param sched;
  int max_pri = sched_get_priority_max (SCHED_RR);

  memset (&sched, 0, sizeof(sched));

  if (pri > max_pri)
    sched.sched_priority = max_pri;
  else
    sched.sched_priority = pri;

  // PID = 0 means the current running program.
  // SCHED_RR = round-robin scheduling policy.
  return sched_setscheduler (0, SCHED_RR, &sched);
}
