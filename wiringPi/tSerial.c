// ****************************************************************************
// tSerial.c
// 
// Build: gcc tSerial.c wiringSerial.o -o tSerial
// ****************************************************************************

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "wiringSerial.h"

// @DEBUG
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// @DEBUG

// ****************************************************************************

#define HERE() do { printf ("%s(%d).%s\r\n", __FILE__, __LINE__, __FUNCTION__); } while(0)
#define DPRINTF(fmt, args...) do { \
        printf ("%s(%d).%s ", __FILE__, __LINE__, __FUNCTION__); \
        printf(fmt, ## args); \
        fflush(stdout); \
} while (0)

// ****************************************************************************

int main(int argc, char *argv[])
{
    // ----------------------------------------------------

    printf ("\r\n\r\n");
    HERE();

    // ----------------------------------------------------

    HERE();
#if 1
    const char devName[] = "/dev/tty";
    int fd = serialOpen(devName, 115200);
#else
    const char devName[] = "test.txt";
    int fd = serialOpen(devName, -1);
#endif
    DPRINTF("fd = %d\r\n", fd);
    if (fd == -1)
    {
        perror("open");
        DPRINTF("ERROR: fd = %d\r\n", fd);
    }

    serialPrintf(fd, "%s(%d).%s HELLO THERE!\r\n", __FILE__, __LINE__, __FUNCTION__);

    serialFlush(fd);
    serialClose(fd);

    // ----------------------------------------------------

    HERE();
    printf ("\r\n");

    return EXIT_SUCCESS;
}

// ****************************************************************************
