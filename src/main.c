#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "rpc.h"
#include "i2c.h"

int main(int args, char* argv[])
{

    i2c_number dev = {
        .i2c_bus_number = 0x00,
        .i2c_device_number = 0x50
    };

    int st;
    int dev_fd = sinae2prom_i2c_open_device(&dev);
    if((st=sinae2prom_rpc_init(dev_fd)) < 0)
    {
        fprintf(stderr, "Failed To Init RPC (%d)\n", st);
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {
        if((st=sinae2prom_rpc_handler()) < 0)
        {
            fprintf(stderr, "Failed To Handle RPC (%d)\n", st);
            close(dev_fd);
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}