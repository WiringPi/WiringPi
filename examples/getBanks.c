/*
 * getBanks.c:
 *	Call digitalReadBank to get values of all GPIOs.
 *
 ***********************************************************************
 * This file is part of wiringPi:
 *      https://github.com/nuncio-bitis/WiringPi
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published
 *by the Free Software Foundation, either version 3 of the License, or (at your
 *option) any later version.
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wiringPi.h>

//**********************************************************************************************************************

int main (void)
{
    printf ("Raspberry Pi Bank Read Test\n");

    wiringPiSetupGpio();

    uint32_t bank0 = digitalReadBank(0);
    uint32_t bank1 = digitalReadBank(1);

    printf("Bank 0: 0x%08X\n", bank0);
    printf("Bank 1: 0x%08X\n", bank1);

    printf("    ");
    for (int i = 31; i >= 0; --i)
    {
        printf("%2d ", i);
    }
    printf("\n");
    printf("    ");
    for (int i = 31; i >= 0; --i)
    {
        printf("-- ");
    }
    printf("\n");

    printf("B0 :");
    for (int i = 31; i >= 0; --i)
    {
        printf("%2d ", (bank0 & (1 << i)) ? 1 : 0);
    }
    printf("\n");

    printf("B1 :");
    for (int i = 31; i >= 0; --i)
    {
        printf("%2d ", (bank1 & (1 << i)) ? 1 : 0);
    }
    printf("\n");

    return EXIT_SUCCESS;
}

//**********************************************************************************************************************
