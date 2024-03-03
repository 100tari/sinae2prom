#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdint.h>

#include "./i2c.h"

int         
sinae2prom_i2c_open_device(i2c_number* i2c_dev)
{
    if(i2c_dev == NULL)
        return I2C_ERR_NULL;

    char i2c_file_path[20];
    int i2c_fd;     

    snprintf(i2c_file_path, 20,"/dev/i2c-%i", i2c_dev->i2c_bus_number);
    if((i2c_fd = open(i2c_file_path, O_RDWR)) < 0)
        return I2C_ERR_OPEN;        

    if(ioctl(i2c_fd, I2C_SLAVE, i2c_dev->i2c_device_number) < 0)
        return -1;

    return i2c_fd;
}

int 
sinae2prom_i2c_read(i2c_number* i2c_dev, uint8_t *r_buff, size_t buff_len)
{
    if(i2c_dev == NULL || r_buff == NULL)
        return I2C_ERR_NULL;

    int i2c_fd;
    if((i2c_fd = sinae2prom_i2c_open_device(i2c_dev)) < 0)
    {
        close(i2c_fd);
        return I2C_ERR_OPEN;
    }

    int msg = i2c_dev->i2c_register_number;
    if(write(i2c_fd, &msg, sizeof(msg)) < 0)
    {
        close(i2c_fd);
        return I2C_ERR_WRITE;
    }

    if(read(i2c_fd, (void*) r_buff, buff_len) != buff_len)
        return I2C_ERR_READ;

    return I2c_ERR_NOERR;
}

int 
sinae2prom_i2c_write(i2c_number* i2c_dev, uint8_t *w_buff, size_t buff_len)
{
    if(i2c_dev == NULL || w_buff == NULL)
        return I2C_ERR_NULL;

    int i2c_fd;
    if((i2c_fd = sinae2prom_i2c_open_device(i2c_dev)) < 0)
    {
        close(i2c_fd);
        return I2C_ERR_OPEN;
    }

    int msg = i2c_dev->i2c_register_number;
    if(write(i2c_fd, &msg, sizeof(msg)) < 0)
    {
        close(i2c_fd);
        return I2C_ERR_WRITE;
    }

    if(write(i2c_fd, (void*) w_buff, buff_len) != buff_len)
        return I2C_ERR_WRITE;

    return I2c_ERR_NOERR;
}