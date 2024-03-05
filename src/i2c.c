#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "./i2c.h" 

#define __DEBUG__(...)       do { if(1) {printf(__VA_ARGS__);}} while(0);       

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
sinae2prom_i2c_read(int i2c_fd, int r_addr, uint8_t *r_buff, size_t buff_len)
{
    if(r_buff == NULL)
        return I2C_ERR_NULL;

    uint8_t mybuf[2];

    mybuf[0] = r_addr >> 8;
    mybuf[1] = r_addr & 0xFF;

    if(write(i2c_fd, &mybuf, sizeof(mybuf)) != 2)
        return I2C_ERR_WRITE;
    
    if(read(i2c_fd, (void*) r_buff, buff_len) != buff_len)
        return I2C_ERR_READ;

    printf("Read %s in %d\n", r_buff, r_addr);

    return I2c_ERR_NOERR;
}

int 
sinae2prom_i2c_write(int i2c_fd, int w_addr, uint8_t *w_buff, size_t buff_len)
{
    if(w_buff == NULL)
        return I2C_ERR_NULL;

    uint8_t mybuf[buff_len + 2];

    mybuf[0] = w_addr >> 8;
    mybuf[1] = w_addr & 0xFF;
    memcpy(&mybuf[2], w_buff, buff_len);

    if(write(i2c_fd, mybuf, buff_len + 2) != (buff_len + 2))
        return I2C_ERR_WRITE;

    return I2c_ERR_NOERR;
}

int 
sinae2prom_e2prom_write(int i2c_dev_fd, int w_addr, uint8_t *w_buff, size_t buff_size)
{
    uint8_t page[E2PROM_PAGE_SIZE];
    int current_addr = w_addr;
    int counter = 0;

    while(counter < buff_size)
    {
        memset(page, 0, E2PROM_PAGE_SIZE);

        int len = (buff_size-counter < E2PROM_PAGE_SIZE) ? buff_size-counter : E2PROM_PAGE_SIZE - current_addr%E2PROM_PAGE_SIZE;
        memcpy(page, &w_buff[counter], len);
        if(sinae2prom_i2c_write(i2c_dev_fd, current_addr, page, len) < 0)
            return I2C_ERR_WRITE;
        
        printf("Wrote %s in %d\n", page, current_addr);

        counter += len;
        current_addr += len;

        sleep(1);
    }
    return I2c_ERR_NOERR;
}