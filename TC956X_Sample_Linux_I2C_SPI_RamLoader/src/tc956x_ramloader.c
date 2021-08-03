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
#include "tc956x_mem.h"


int main( int argc, char* argv[] )
{
    int sts;
    int ret = 0;

    if( argc < 2 ) {
        printf("Usage: %s filename.bin\n", argv[0]);
        return 0;
    }

    sts = TC956X_START_REG_ACC();
    if( sts != 0 ) {
        ret = -1;
        goto EXIT;
    }

    ret = tc956x_load_fw( argv[1] );

EXIT:

    TC956X_END_REG_ACC();

    return ret;
}
