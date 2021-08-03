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
#include "tc956x_reg.h"

#if (TC956X_REG_IF_TYPE == 1)
static char *reg_acc_if= "I2C";
#elif (TC956X_REG_IF_TYPE == 2)
static char *reg_acc_if= "SPI";
#endif


#ifdef TC9560
// Reset Control
#define NRSTCTRL (0x1008)
// Sidband Ram Access registers
#define NMEMOFST (0x1100)
#define NMEMVAL  (0x1104)
#endif

#ifdef TC956X
#define BMX_IMEM_BASE 0x00000000
#define BMX_DMEM_BASE 0x00400000
#define BMX_SFR_BASE  0x00800000
#define NRSTCTRL (BMX_SFR_BASE + 0x1008)
#endif

#define RELEASE_RESET_CM3
#define VERIFY_FW


#ifdef TC9560
int tc956x_read_ram( uint32_t ramAddr, const uint8_t *pData, uint32_t size )
{
        fprintf(stderr, "TC9560 does not support SRAM read through %s bus\n", reg_acc_if);
        return -1;
}

int tc956x_write_ram( uint32_t ramAddr, const uint8_t *pData, uint32_t size )
{
    int ret = 0;
    int sts;
    uint32_t idx;

    // Set Sideband SRAM load pointer
    sts = TC956X_REG_WR( NMEMOFST, ramAddr );
    if(sts != 0) {
        fprintf(stderr, "%s bus access error\n", reg_acc_if);
        ret = -1;
        goto EXIT;
    }

    
    for(idx = 0; idx < size; idx += 4) {
        if(size - idx >= 4) {
            sts = TC956X_REG_WR_B( NMEMVAL, &pData[idx] );
        } else {
            uint32_t i;
        	uint8_t buf[4] = {0,0,0,0};
            for(i=0; i< size - idx; i++){
                buf[i] = pData[idx+i];
            }
            sts = TC956X_REG_WR_B( NMEMVAL, buf );
        }
        if(sts != 0){
            fprintf(stderr, "%s bus access error\n", reg_acc_if);
            ret = -1;
            goto EXIT;
        }
    }

EXIT:
	return ret;
}
#endif

#ifdef TC956X
int tc956x_read_ram( uint32_t ramAddr, uint8_t *pData, uint32_t size )
{
    int ret = 0;
    int sts;
    uint32_t idx;
    uint32_t base_addr;

    base_addr = BMX_IMEM_BASE + (ramAddr & 0xfffffffc);
    for(idx = 0; idx < size; idx += 4) {
        if(size - idx >= 4) {
            sts = TC956X_REG_RD_B( base_addr + idx, &pData[idx] );
        } else {
            uint32_t i;
            uint8_t buf[4];
            sts = TC956X_REG_RD_B( base_addr + idx, buf );
            for(i=0; i< size - idx; i++){
                pData[idx+i] = buf[i];
            }
        }
        if(sts != 0){
            fprintf(stderr, "%s bus access error\n", reg_acc_if);
            ret = -1;
            goto EXIT;
        }
    }

EXIT:
    return ret;
}

int tc956x_write_ram( uint32_t ramAddr, const uint8_t *pData, uint32_t size )
{
    int ret = 0;
    int sts;
    uint32_t idx;
    uint32_t base_addr;

    base_addr = BMX_IMEM_BASE + (ramAddr & 0xfffffffc);
    for(idx = 0; idx < size; idx += 4) {
        if(size - idx >= 4) {
            sts = TC956X_REG_WR_B( base_addr + idx, &pData[idx] );
        } else {
            uint32_t i;
            uint8_t buf[4] = {0,0,0,0};
            for(i=0; i< size - idx; i++){
                buf[i] = pData[idx+i];
            }
            sts = TC956X_REG_WR_B( base_addr + idx, buf );
        }
        if(sts != 0){
            fprintf(stderr, "%s bus access error\n", reg_acc_if);
            ret = -1;
            goto EXIT;
        }
    }

EXIT:
    return ret;
}
#endif

int tc956x_load_fw(const char *filename)
{
    int ret = 0;
    int sts;
    FILE *fp = NULL;
    uint8_t *dataBuf = NULL;
    uint8_t *verifBuf = NULL;
    uint32_t dataSize;
    uint32_t regVal;

    fp = fopen(filename, "rb");
    if( fp == NULL ) {
        fprintf(stderr, "Cannot open %s\n", filename);
        ret = -1;
        goto EXIT;
    }
    fseek(fp, 0, SEEK_END);
    dataSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    fprintf(stdout, "%s opened. size = %u\n", filename, dataSize);

    dataBuf = malloc(dataSize);
    if( dataBuf == NULL ) {
        fprintf(stderr, "Cannot allocate buffer\n");
        ret = -1;
        goto EXIT;
    }

    if( fread(dataBuf, 1, dataSize, fp) != dataSize ) {
        fprintf(stderr, "Cannot read fw\n");
        ret = -1;
        goto EXIT;
    }


    sts = tc956x_write_ram( 0x000000, dataBuf, dataSize );
    if( sts != 0 ) {
        fprintf(stderr, "Failed to write SRAM\n");
        ret = -1;
        goto EXIT;
    }

#if defined(TC956X) && defined(VERIFY_FW)
{   // Verify FW. TC9560 cannot do this.
    uint32_t i;
    verifBuf = malloc(dataSize);
    if( verifBuf == NULL ) {
        fprintf(stderr, "Cannot allocate buffer\n");
        ret = -1;
        goto EXIT;
    }
    
    sts = tc956x_read_ram( 0x000000, verifBuf, dataSize );
    if( sts != 0 ) {
        fprintf(stderr, "Failed to read SRAM\n");
        ret = -1;
        goto EXIT;
    }

    for( i = 0; i < dataSize; i++ ) {
        if( verifBuf[i] != dataBuf[i] ) {
            fprintf(stderr, "Verify Error\n");
            fprintf(stderr, "%06x: %02x %02x\n", i, verifBuf[i], dataBuf[i]);
            ret = -1;
            goto EXIT;
        }
    }
    fprintf( stdout, "%u bytes verified\n", dataSize );
}
#endif

#ifdef RELEASE_RESET_CM3
    // Deassert reset
    sts = TC956X_REG_RD( NRSTCTRL, &regVal );
    regVal |= 0x03;
    sts |= TC956X_REG_WR( NRSTCTRL, regVal );
    regVal &= ~0x03;
    sts |= TC956X_REG_WR( NRSTCTRL, regVal );
    if(sts != 0){
        fprintf(stderr, "Cannot reset CM3\n");
        ret = -1;
    }
#endif

EXIT:
	if( fp ) {
        fclose(fp);
    }

    if( dataBuf ) {
        free(dataBuf);
    }
    
    if( verifBuf ) {
        free(verifBuf);
    }
    
    return ret;
}

/* End of file */
