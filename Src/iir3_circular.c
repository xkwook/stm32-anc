/*
 * iir3_circular.c
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "iir3_circular.h"

#include <string.h>

/* Private methods declaration */

static inline q15_t calculate_stage0(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a);
static inline q15_t calculate_stage1(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a);
static inline q15_t calculate_stage2(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a);
static inline q15_t calculate_stage3(q15_t* x, q15_t* y, q15_t* b, q15_t* a);

/* Public methods definition */

void iir3_circular_init(
    iir3_circular_t*    self,
    q15_t*              b_coeffs_p,
    q15_t*              a_coeffs_p,
    q15_t*              oldDataIn_p,
    q15_t*              oldDataOut_p
)
{
    self->b_coeffs_p    = b_coeffs_p;
    self->a_coeffs_p    = a_coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->oldDataOut_p  = oldDataOut_p;

    memset(self->dataIn, 0, sizeof(self->dataIn));
    memset(self->dataOut, 0, sizeof(self->dataOut));
}

inline q15_t* iir3_circular_getDataInPtr(iir3_circular_t* self)
{
    return self->dataIn;
}

inline q15_t* iir3_circular_getDataOutPtr(iir3_circular_t* self)
{
    return self->dataOut;
}

inline void iir3_circular_calculate(iir3_circular_t* self)
{
    q15_t* x  = self->dataIn;
    q15_t* y  = self->dataOut;
    q15_t* x_ = self->oldDataIn_p;
    q15_t* y_ = self->oldDataOut_p;
    q15_t* b  = self->b_coeffs_p;
    q15_t* a  = self->a_coeffs_p;

    y[0] = calculate_stage0(x, y, x_, y_, b, a);
    y[1] = calculate_stage1(x, y, x_, y_, b, a);
    y[2] = calculate_stage2(x, y, x_, y_, b, a);
    for (uint32_t i = 0; i < (IIR3_CIRCULAR_DATA_CHUNK - IIR3_CIRCULAR_MIN_CHUNK); i++)
    {
        y[i + IIR3_CIRCULAR_MIN_CHUNK] = calculate_stage3(x + i, y + i, b, a);
    }
}

/* Private methods definition */

static inline q15_t calculate_stage0(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a)
{
    q15_t result;

    q31_t acc0, acc1, sum0;

    const uint32_t END = IIR3_CIRCULAR_DATA_CHUNK - 1;

    acc0 = 0;
    acc1 = 0;

    /* Perform the multiply-accumulate for x(n) ... x(n-3) */
    acc0 += b[0] * x[0];
    acc1 += b[1] * x_[END];

    acc0 += b[2] * x_[END - 1];
    acc1 += b[3] * x_[END - 2];

    /* Perform the multiply-accumulate for y(n-1) ... y(n-3) */
    acc0 += a[0] * y_[END];
    acc1 += a[1] * y_[END - 1];

    acc0 += a[2] * y_[END - 2];

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    return result;
}

static inline q15_t calculate_stage1(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a)
{
    q15_t result;

    q31_t acc0, acc1, sum0;

    const uint32_t END = IIR3_CIRCULAR_DATA_CHUNK - 1;

    acc0 = 0;
    acc1 = 0;

    /* Perform the multiply-accumulate for x(n) ... x(n-3) */
    acc0 += b[0] * x[1];
    acc1 += b[1] * x[0];

    acc0 += b[2] * x_[END];
    acc1 += b[3] * x_[END - 1];

    /* Perform the multiply-accumulate for y(n-1) ... y(n-3) */
    acc0 += a[0] * y[0];
    acc1 += a[1] * y_[END];

    acc0 += a[2] * y_[END - 1];

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    return result;
}

static inline q15_t calculate_stage2(q15_t* x, q15_t* y, q15_t* x_, q15_t* y_, q15_t* b, q15_t* a)
{
    q15_t result;

    q31_t acc0, acc1, sum0;

    const uint32_t END = IIR3_CIRCULAR_DATA_CHUNK - 1;

    acc0 = 0;
    acc1 = 0;

    /* Perform the multiply-accumulate for x(n) ... x(n-3) */
    acc0 += b[0] * x[2];
    acc1 += b[1] * x[1];

    acc0 += b[2] * x[0];
    acc1 += b[3] * x_[END];

    /* Perform the multiply-accumulate for y(n-1) ... y(n-3) */
    acc0 += a[0] * y[1];
    acc1 += a[1] * y[0];

    acc0 += a[2] * y_[END];

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    return result;
}

static inline q15_t calculate_stage3(q15_t* x, q15_t* y, q15_t* b, q15_t* a)
{
    q15_t result;

    q31_t acc0, acc1, sum0;

    const uint32_t END = IIR3_CIRCULAR_DATA_CHUNK - 1;

    acc0 = 0;
    acc1 = 0;

    /* Perform the multiply-accumulate for x(n) ... x(n-3) */
    acc0 += b[0] * x[3];
    acc1 += b[1] * x[2];

    acc0 += b[2] * x[1];
    acc1 += b[3] * x[0];

    /* Perform the multiply-accumulate for y(n-1) ... y(n-3) */
    acc0 += a[0] * y[2];
    acc1 += a[1] * y[1];

    acc0 += a[2] * y[0];

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    return result;
}