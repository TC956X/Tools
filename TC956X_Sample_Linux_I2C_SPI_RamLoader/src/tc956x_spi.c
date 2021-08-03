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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static int spi_fd = -1;
static unsigned char spi_mode;
static unsigned char spi_bitsPerWord;
static unsigned int spi_speed;
static const char *spi_device = "/dev/spidev0.0";

int tc956x_open_spi(void)
{
    int status_value = -1;
	
    if ( spi_fd != -1 ) {
        return -1;
    }

    //----- SET SPI MODE -----
    //SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
    //SPI_MODE_1 (0,1) 	CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_2 (1,0) 	CPOL = 1, CPHA = 0, Clock idle high, data is clocked in on falling edge, output data (change) on rising edge
    //SPI_MODE_3 (1,1) 	CPOL = 1, CPHA = 1, Clock idle high, data is clocked in on rising, edge output data (change) on falling edge
    spi_mode = SPI_MODE_0;
    
    //----- SET BITS PER WORD -----
    spi_bitsPerWord = 8;
    
    //----- SET SPI BUS SPEED -----
    spi_speed = 5000000;		//5MHz (200nS per bit) 

   	spi_fd = open(spi_device, O_RDWR);
    if (spi_fd < 0) {
        perror("Error - Could not open SPI device");
        exit(1);
	}

    status_value = ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode);
    if(status_value < 0)
    {
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }

    status_value = ioctl(spi_fd, SPI_IOC_RD_MODE, &spi_mode);
    if(status_value < 0)
    {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
    if(status_value < 0)
    {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }

    status_value = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if(status_value < 0)
    {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }
    return 0;
}

int tc956x_close_spi(void)
{
	int status_value = 0;

	if ( spi_fd != -1 ) {
	    status_value = close(spi_fd);
	    spi_fd = -1;
	}

    if(status_value < 0)
    {
    	perror("Error - Could not close SPI device");
    	exit(1);
    }
    return 0;
}

int tc956x_spi_send( uint8_t *txbuf, uint8_t *rxbuf, uint32_t len)
{
	int retVal = -1;
	struct spi_ioc_transfer spi;

	memset(&spi, 0, sizeof(spi));
	
	spi.tx_buf        = (unsigned long)txbuf;
	spi.rx_buf        = (unsigned long)rxbuf;
	spi.len           = len;
	spi.delay_usecs   = 0;
	spi.speed_hz      = spi_speed;
	spi.bits_per_word = spi_bitsPerWord;
	spi.cs_change = 0;
	spi.pad = 0;		// fill .pad with 0

	retVal = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi) ;
	if(retVal < 0)
	{
		perror("Error - Problem transmitting spi data..ioctl");
		return -1;
	}
    return 0;
}

int tc956x_spi_write_reg( uint32_t regAddr, uint32_t regVal )
{
	uint8_t buf[7];
	int ret;
	regAddr &= 0xfffffffc;
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8 );
	buf[2] = (uint8_t)(regAddr | 0x0);//write cmd
	buf[3] = (uint8_t)(regVal >> 24);
	buf[4] = (uint8_t)(regVal >> 16);
	buf[5] = (uint8_t)(regVal >> 8);
	buf[6] = (uint8_t)(regVal);
	ret = tc956x_spi_send( buf, NULL, 7 );
	return ret;
}

int tc956x_spi_write_reg_bin( uint32_t regAddr, const uint8_t *pData )
{
	uint8_t buf[7];
	int ret;
	regAddr &= 0xfffffffc;
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8 );
	buf[2] = (uint8_t)(regAddr | 0x0);//write cmd
	buf[3] = pData[3];
	buf[4] = pData[2];
	buf[5] = pData[1];
	buf[6] = pData[0];
	ret = tc956x_spi_send( buf, NULL, 7 );
	return ret;
}

int tc956x_spi_read_reg( uint32_t regAddr, uint32_t *pRegVal)
{
	uint8_t buf[7]={0};
	uint8_t rxbuf[7]={0};
	int ret;
	
	regAddr &= 0xfffffffc;
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8 );
	buf[2] = (uint8_t)(regAddr | 0x1);//read cmd
	ret = tc956x_spi_send( buf, NULL, 7 );
	ret |= tc956x_spi_send( buf, rxbuf, 7 );
	if(pRegVal) {
	    *pRegVal = (rxbuf[3]<<24) + (rxbuf[4]<<16) + (rxbuf[5]<<8) + rxbuf[6];
    }
    return ret;
}

int tc956x_spi_read_reg_bin( uint32_t regAddr, uint8_t *pData )
{
	uint8_t buf[7]={0};
	uint8_t rxbuf[7]={0};
	int ret;
	
	regAddr &= 0xfffffffc;
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8 );
	buf[2] = (uint8_t)(regAddr | 0x1);//read cmd
	ret = tc956x_spi_send( buf, NULL, 7 );
	ret |= tc956x_spi_send( buf, rxbuf, 7 );
	if(pData) {
		pData[0] = rxbuf[6];
		pData[1] = rxbuf[5];
		pData[2] = rxbuf[4];
		pData[3] = rxbuf[3];
    }
    return ret;
}

int tc956x_spi_read_reg_n( uint32_t regAddr, uint32_t *pRegVal, int num )
{
	uint8_t buf[7]={0};
	uint8_t rxbuf[7]={0};
	int ret, sts;
	int i;

    ret = -1;
    if(pRegVal == NULL || num < 0) {
        return -1;
    }
	regAddr &= 0xfffffffc;
	buf[0] = (uint8_t)(regAddr >> 16);
	buf[1] = (uint8_t)(regAddr >> 8 );
	buf[2] = (uint8_t)(regAddr | 0x1);//read cmd
	sts = tc956x_spi_send( buf, NULL, 7 );
	if( sts != 0 ){
        ret = -1;
        goto EXIT;
    }
	for(i = 0; i < num; i++){
        regAddr += 4;
        buf[0] = (uint8_t)(regAddr >> 16);
        buf[1] = (uint8_t)(regAddr >> 8 );
        buf[2] = (uint8_t)(regAddr | 0x1);//read cmd
        sts = tc956x_spi_send( buf, rxbuf, 7 );
        if( sts != 0 ){
            ret = -1;
            goto EXIT;
        }
        pRegVal[i] = (rxbuf[3]<<24) + (rxbuf[4]<<16) + (rxbuf[5]<<8) + rxbuf[6];
    }

EXIT:
    return ret;
}
