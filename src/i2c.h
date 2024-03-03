#ifndef SINAE2PROM_I2C_H
#define SINAE2PROM_I2C_H

#include <stdint.h>
#include <sys/types.h>

typedef struct 
{
    int     i2c_bus_number;
    int     i2c_device_number;
} i2c_number;

enum
{
    I2c_ERR_NOERR   =    1,
    I2C_ERR_NULL    =   -2,
    I2C_ERR_OPEN    =   -3,
    I2C_ERR_WRITE   =   -4,
    I2C_ERR_READ    =   -5
};

int             sinae2prom_i2c_open_device(i2c_number*);
int             sinae2prom_i2c_read(i2c_number*, uint8_t*, size_t);
int             sinae2prom_i2c_read(i2c_number*, uint8_t*, size_t);


#endif // SINAE2PROM_I2C_H