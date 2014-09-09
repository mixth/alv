#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include "source.h"

const int HMC5883L_I2C_ADDR = 0x1E;

void selectDevice(int fd, int addr, char * name)
{
    if (ioctl(fd, I2C_SLAVE, addr) < 0)
        printf("%s not present\n", name);
}

void writeToDevice(int fd, int reg, int val)
{
    char buf[2];
    buf[0]=reg;
    buf[1]=val;
    if (write(fd, buf, 2) != 2)
        printf("Can't write to ADXL345\n");
}

float main_compass()
{
    int fd,i;
    float angle;
    short x,y,z;
    unsigned char buf[16];
    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0){
        printf("Failed to open i2c bus\n");
        return -200;
    }
    selectDevice(fd, HMC5883L_I2C_ADDR, "HMC5883L");
    writeToDevice(fd, 0x01, 32);
    writeToDevice(fd, 0x02, 0);  
    buf[0] = 0x03;
    if ((write(fd, buf, 1)) != 1){
        printf("Error writing to i2c slave\n");
        return -200;
    }
    if (read(fd, buf, 6) != 6) {
        printf("Unable to read from HMC5883L\n");
        return -200;
    }
    else{
        x = (buf[0] << 8) | buf[1];
        y = (buf[4] << 8) | buf[5];
        z = (buf[2] << 8) | buf[3];
        angle = atan2(y, x) * 180 / 3.14159265;
    }
    close(fd);
    return angle;
}
