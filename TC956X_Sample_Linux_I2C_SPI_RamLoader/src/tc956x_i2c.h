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


#ifndef _TC956X_I2C_H_INCLUDED
#define _TC956X_I2C_H_INCLUDED

#define TC956X_SLAVE_ADDR  (0x77)
//#define TC956X_SLAVE_ADDR  (0x33)
#define TC956X_I2C_DEV	"/dev/i2c-%d"

int tc956x_i2c_init(void);
int tc956x_i2c_deinit(void);
int tc956x_i2c_read_reg( uint32_t regAddr, uint32_t *pData );
int tc956x_i2c_read_reg_bin( uint32_t regAddr, uint8_t *pData);
int tc956x_i2c_write_reg( uint32_t regAddr, uint32_t regVal);
int tc956x_i2c_write_reg_bin( uint32_t regAddr, const uint8_t *pData);


#endif
