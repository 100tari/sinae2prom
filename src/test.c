#include "./i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int args, char* argv[])
{
    if(args != 4 || ((args == 2 && strcmp(argv[1], "--help") == 0)))
    {
        printf("Usage: %s <w/r> <addr> <dataToWrite/bytesToRead>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    i2c_number dev = {
        .i2c_bus_number = 0x00,
        .i2c_device_number = 0x50
    };
    int fd = sinae2prom_i2c_open_device(&dev);

    if(strcmp(argv[1], "w") == 0)
    {
        if(sinae2prom_e2prom_write(fd, atoi(argv[2]) , argv[3], strlen(argv[3])) != I2c_ERR_NOERR)
        {
            printf("Failed To Write\nTerminating...\n");
            exit(EXIT_FAILURE);
        } printf("Write Done Successfully : Wrote -> %s\n", argv[3]);
    }

    if(strcmp(argv[1], "r") == 0)
    {
        char buf[atoi(argv[3])];

        if(sinae2prom_e2prom_read(fd, atoi(argv[1]) , buf, atoi(argv[3])) != I2c_ERR_NOERR)
        {
            printf("Failed To Read\nTerminating...\n");
            exit(EXIT_FAILURE);
        }   printf("Read Done Successfully : Read -> %s\n", buf);
    }
}