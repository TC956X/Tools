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


#ifndef _TC956X_REG_H_INCLUDED
#define _TC956X_REG_H_INCLUDED

#include "tc956x.h"
#include "tc956x_i2c.h"
#include "tc956x_spi.h"

#if (TC956X_REG_IF_TYPE==1)

#define TC956X_START_REG_ACC()             tc956x_i2c_init()
#define TC956X_END_REG_ACC()               tc956x_i2c_deinit()
#define TC956X_REG_WR(addr, val)           tc956x_i2c_write_reg(addr, val)
#define TC956X_REG_WR_B(addr, pData)       tc956x_i2c_write_reg_bin(addr, pData)
#define TC956X_REG_RD(addr, pVal)          tc956x_i2c_read_reg(addr, pVal)
#define TC956X_REG_RD_B(addr, pData)       tc956x_i2c_read_reg_bin(addr, pData)

#elif (TC956X_REG_IF_TYPE==2)

#define TC956X_START_REG_ACC()             tc956x_open_spi()
#define TC956X_END_REG_ACC()               tc956x_close_spi()
#define TC956X_REG_WR(addr, val)           tc956x_spi_write_reg(addr, val)
#define TC956X_REG_WR_B(addr, pData)       tc956x_spi_write_reg_bin(addr, pData)
#define TC956X_REG_RD(addr, pVal)          tc956x_spi_read_reg(addr, pVal)
#define TC956X_REG_RD_B(addr, pData)       tc956x_spi_read_reg_bin(addr, pData)

#else
#error TC956X_REG_IF_TYPE should be 1(I2C) or 2(SPI)
#endif

#define TC956X_SRAM_WR(addr, pData, size)  tc956x_write_ram(addr, (uint8_t*)(pData), size)

#endif
