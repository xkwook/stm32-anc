/*
 * fir_circular.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "fir_circular.h"

#include <string.h>

/* Public methods definiton */

void fir_circular_init(
    fir_circular_t* self,
    q15_t*          coeffs_p,
    q15_t*          oldDataIn_p,
    q15_t*          stateBfr_p,
    uint32_t        length
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->stateBfr_p    = self->stateBfr_p;
    self->length        = self->length;

    memset(self->stateBfr_p, 0, self->length * sizeof(self->stateBfr_p[0]));
}

inline void fir_circular_pushData(
    fir_circular_t* self,
    q15_t           dataIn
)
{
    self->dataIn        = dataIn;

    /* Put new and old data to state buffer */
    self->stateBfr_p[0] = dataIn;
    self->stateBfr_p[1] = *(self->oldDataIn_p);
}

inline q15_t* fir_circular_getDataInPtr(
    fir_circular_t* self
)
{
    return &(self->dataIn);
}

inline q15_t fir_circular_calculate(
    fir_circular_t* self
)
{
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t* c0_p;
    q15_t* c1_p;
    q15_t  x0, x1;
    q15_t  c0, c1;
    q31_t  acc0, acc1, sum0;
    q15_t  result;
    uint32_t tapCnt;
    uint32_t secondHalfIdx;
    uint32_t n;

    /* Load n */
    n = self->length;

    secondHalfIdx = n >> 1u;

    /* Zero accumulators */
    acc0 = 0;
    acc1 = 0;

    /* Init state pointers */
    x0_p = self->stateBfr_p;
    x1_p = self->stateBfr_p + secondHalfIdx;

    /* Init coeffs pointers */
    c0_p = self->coeffs_p;
    c1_p = self->coeffs_p + secondHalfIdx;

    /* Set tapCnt to blocks of 8 samples */
    tapCnt = n >> 3u;

    /* Process blocks of 8 samples */
    while (tapCnt > 0u)
    {
        /* Load state variables */
        x0 = *x0_p++;
        x1 = *x1_p++;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p++;
        x1 = *x1_p++;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p++;
        x1 = *x1_p++;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p++;
        x1 = *x1_p++;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Decrement tapCnt */
        tapCnt--;
    }

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    return result;
}
