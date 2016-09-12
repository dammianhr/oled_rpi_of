//
//  I2Cdev.h
//  i2c Interface with 16bit address register
//  ioctl() method
//
//  Created by Damian Nahmiyas on 30/1/16.
//
//

#ifndef i2cSender__I2Cdev_h
#define i2cSender__I2Cdev_h
#include "ofMain.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

class I2Cdev{
public:
    /*---------------------------------------------------------------------
     Init I2Cdevice
     ---------------------------------------------------------------------*/
    I2Cdev(const char * deviceName)
    {
        fd = open(deviceName, O_RDWR);
        if (fd == -1)
            ofLog(OF_LOG_ERROR,"Failed to open I2C device.");
    }
    ~I2Cdev()
    {
        close(fd);
    }
    
    void addressSet(uint8_t address)
    {
        device_addr = address;
        long func;
        if(ioctl(fd,I2C_FUNCS, &func)<0)
            ofLog(OF_LOG_ERROR,"Failed - ioctl(fd,I2C_FUNCS, &func)");
    }
    
    /*---------------------------------------------------------------------
     Read n bytes
     ---------------------------------------------------------------------*/
    void readBlock(uint16_t command, uint8_t size, uint8_t * data)
    {
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg message[2];
        uint8_t my_buf[2];
        
        my_buf[0] = (command>>8);
        my_buf[1] = (uint8_t)command;
        
        message[0].addr  = device_addr;
        message[0].flags = 0;
        message[0].len   = 2;
        message[0].buf   = my_buf;
        
        message[1].addr  = device_addr;
        message[1].flags = I2C_M_RD;
        message[1].len   = size;
        message[1].buf   = data;
        
        msg_rdwr.msgs = message;
        msg_rdwr.nmsgs = 2;
        
        if(ioctl(fd,I2C_RDWR,&msg_rdwr)<0)
            ofLog(OF_LOG_ERROR, "Failed to reading block data");
    }
    
    /*---------------------------------------------------------------------
     Write n bytes
     ---------------------------------------------------------------------*/
    void writeBlock(uint16_t command, uint16_t size, uint8_t * data)
    {
        struct i2c_rdwr_ioctl_data msg_rdwr;
        struct i2c_msg i2cmsg;
        uint8_t my_buf[size + 2];
        my_buf[0] = (command>>8);
        my_buf[1] = (uint8_t)command;
        
        for(int i= 0; i < size; i++)
            my_buf[2+i] = data[i];
        
        msg_rdwr.msgs = &i2cmsg;
        msg_rdwr.nmsgs = 1;
        
        i2cmsg.addr  = device_addr;
        i2cmsg.flags = 0;
        i2cmsg.len   = 2+size;
        i2cmsg.buf   = my_buf;
        
        if(ioctl(fd,I2C_RDWR,&msg_rdwr)<0)
            ofLog(OF_LOG_ERROR, "Failed to writing block data");
    }
    
private:
    int fd;
    uint8_t device_addr;
};

#endif

//--------------------------------------------------------------------//
// in RPI, enabled to make it perform repeated start correctly
// sudo su -
// echo -n 1 > /sys/module/i2c_bcm2708/parameters/combined
// exit
//-------------------------------------------------------------------//


