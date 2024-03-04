#include "./i2c.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int args, char* argv[])
{
    i2c_number dev = {
        .i2c_bus_number = 0x00,
        .i2c_device_number = 0x50
    };
    int fd = sinae2prom_i2c_open_device(&dev);

    sinae2prom_e2prom_write(fd, atoi(argv[1]) , argv[2], strlen(argv[2]));
}