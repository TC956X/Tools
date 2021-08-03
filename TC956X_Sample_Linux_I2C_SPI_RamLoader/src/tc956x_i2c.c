/* ============================================================================
 * The MIT License (MIT)
 *
 * Copyright (C) 2018-2021  Toshiba Electronic Devices & Storage Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * ========================================================================= */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "tc956x_i2c.h"

// Raspberry Pi I2C does not support repeated start default
// To enable Sr, make /etc/modprobe.d/i2c.conf
// write the following
// options i2c_bcm2708 combined=1

typedef struct tagtc956xI2cData{
	int i2c_fd;
} tc956xI2cData_t;

static tc956xI2cData_t prvData = {
	-1,
};


int tc956x_i2c_init(void)
{
	int ret = 0;
	tc956xI2cData_t *p = &prvData;
	char dev_name[128];

	sprintf(dev_name, "/dev/i2c-%d", DEV_I2C);
	p->i2c_fd = open(dev_name, O_RDWR);
	if( p->i2c_fd < 0){
		perror(dev_name);
		ret = -1;
		goto EXIT;
	}

    // Set slave addr
	if( ioctl(p->i2c_fd, I2C_SLAVE, TC956X_SLAVE_ADDR ) < 0 ) {
		perror("Unable to get bus access to talk to slave.\n");
		ret = -1;
		goto EXIT;
	}

EXIT:
	if( ret != 0){
		if(p->i2c_fd >= 0) {
			close( p->i2c_fd );
			p->i2c_fd = -1;
		}
	}
	return ret;
}


int tc956x_i2c_deinit(void)
{
	tc956xI2cData_t *p = &prvData;

	if( p->i2c_fd  >= 0 ) {
		close(p->i2c_fd);
		p->i2c_fd = -1;
	}

	return 0;
}


int tc956x_i2c_read_reg( uint32_t regAddr, uint32_t *pData )
{
	tc956xI2cData_t *p = &prvData;
	int ret = 0;
	int sts;
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msg[2];

	uint8_t addr_buf[3] = {0};
	uint8_t val_buf[4] = {0};

	if(p->i2c_fd < 0){
		printf("Error I2C bus not opened\n");
		ret = -1;
		goto EXIT;
	}

	memset(&rdwr, 0, sizeof(rdwr));
	memset(msg, 0, sizeof(msg));

	addr_buf[0] = (uint8_t)(regAddr >> 16);
	addr_buf[1] = (uint8_t)(regAddr >> 8 );
	addr_buf[2] = (uint8_t)(regAddr);
	msg[0].addr = TC956X_SLAVE_ADDR;
	msg[0].flags = 0;
	msg[0].len = 3;
	msg[0].buf = addr_buf;

	msg[1].addr = TC956X_SLAVE_ADDR;
	msg[1].flags = I2C_M_RD;
	msg[1].len = 4;
	msg[1].buf = val_buf;

	rdwr.msgs = msg;
	rdwr.nmsgs = 2;

	sts = ioctl(p->i2c_fd, I2C_RDWR, &rdwr);
	if( sts != rdwr.nmsgs ){
		printf( "Error writing to I2C slave(%d)\n",sts);
		perror( "" );
		ret = -1;
		goto EXIT;
	}

//	printf("ADDR= 0x%06x: Data= %02x%02x%02x%02x\n", regAddr, val_buf[3], val_buf[2], val_buf[1], val_buf[0]);

	if( pData ) {
		*pData = (val_buf[3]<<24) + (val_buf[2]<<16) + (val_buf[1]<<8) + val_buf[0];
	}

EXIT:
	return ret;
}

int tc956x_i2c_read_reg_bin( uint32_t regAddr, uint8_t *pData)
{
	tc956xI2cData_t *p = &prvData;
	int ret = 0;
	int sts;
	struct i2c_rdwr_ioctl_data rdwr;
	struct i2c_msg msg[2];

	uint8_t addr_buf[3] = {0};
	uint8_t val_buf[4] = {0};

	if(p->i2c_fd < 0){
		printf("Error I2C bus not opened\n");
		ret = -1;
		goto EXIT;
	}

	memset(&rdwr, 0, sizeof(rdwr));
	memset(msg, 0, sizeof(msg));

	addr_buf[0] = (uint8_t)(regAddr >> 16);
	addr_buf[1] = (uint8_t)(regAddr >> 8 );
	addr_buf[2] = (uint8_t)(regAddr);
	msg[0].addr = TC956X_SLAVE_ADDR;
	msg[0].flags = 0;
	msg[0].len = 3;
	msg[0].buf = addr_buf;

	msg[1].addr = TC956X_SLAVE_ADDR;
	msg[1].flags = I2C_M_RD;
	msg[1].len = 4;
	msg[1].buf = val_buf;

	rdwr.msgs = msg;
	rdwr.nmsgs = 2;

	sts = ioctl(p->i2c_fd, I2C_RDWR, &rdwr);
	if( sts != rdwr.nmsgs ){
		printf( "Error writing to I2C slave(%d)\n",sts);
		perror( "" );
		ret = -1;
		goto EXIT;
	}

//	printf("ADDR= 0x%06x: Data= %02x%02x%02x%02x\n", regAddr, val_buf[3], val_buf[2], val_buf[1], val_buf[0]);

	if( pData ) {
		pData[0] = val_buf[0];
		pData[1] = val_buf[1];
		pData[2] = val_buf[2];
		pData[3] = val_buf[3];
	}

EXIT:
	return ret;
}

int tc956x_i2c_write_reg( uint32_t regAddr, uint32_t regVal)
{
	int ret = 0;
	int sts;
	uint8_t buf[8];
	tc956xI2cData_t *p = &prvData;

    memset(buf, 0, sizeof(buf));

    // Big Endien for reg addr
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8);
	buf[2] = (uint8_t)regAddr;

    // Little Endien for reg val
	buf[3] = (uint8_t)(regVal);
	buf[4] = (uint8_t)(regVal >> 8);
	buf[5] = (uint8_t)(regVal >> 16);
	buf[6] = (uint8_t)(regVal >> 24);

	sts = write(p->i2c_fd, buf, 7);
	if( sts != 7 ){
		printf( "Error writing to I2C slave(%d)\n",sts);
		perror( "" );
		ret = -1;
		goto EXIT;
	} else {
//		printf( "%08x\n", regVal);
	}

EXIT:
	return ret;
}


int tc956x_i2c_write_reg_bin( uint32_t regAddr, const uint8_t *pData)
{
	int ret = 0;
	int sts;
	uint8_t buf[8];
	tc956xI2cData_t *p = &prvData;

    memset(buf, 0, sizeof(buf));

    // Big Endien for reg addr
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8);
	buf[2] = (uint8_t)regAddr;

	buf[3] = pData[0];
	buf[4] = pData[1];
	buf[5] = pData[2];
	buf[6] = pData[3];

	sts = write(p->i2c_fd, buf, 7);
	if( sts != 7 ){
		printf( "Error writing to I2C slave(%d)\n",sts);
		perror( "" );
		ret = -1;
		goto EXIT;
	}

EXIT:
	return ret;
}




