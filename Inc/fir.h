/*
 * fir_circular.h
 *
 *  Created on: Nov 1, 2020
 *      Author: klukomski
 */

#ifndef FIR_H_
#define FIR_H_

#include "anc_math.h"

struct fir_struct
{
    volatile q15_t* coeffs_p;
    volatile q15_t* stateBfr_p;
    uint32_t        length;
};

typedef volatile struct fir_struct fir_t;

/* Public methods declaration */

void fir_init(
    fir_t*          self,
    volatile q15_t* coeffs_p,
    volatile q15_t* stateBfr_p,
    uint32_t        length
);

static inline void fir_pushData(
    fir_t*          self,
    q15_t           dataIn
)
{
    uint32_t n;

    /* Set new data in variable */
    self->dataIn        = dataIn;

    /* Load length */
    n = self->length;

    /* Put new data to state buffer */
    self->stateBfr_p[n - 1] = dataIn;
}

static inline q15_t fir_calculate(
    fir_t* self
)
{
    volatile q15_t* x0_p;
    volatile q15_t* x1_p;
    volatile q15_t* c0_p;
    volatile q15_t* c1_p;
    q15_t  x0, x1;
    q15_t  c0, c1;
    q63_t  acc0, acc1, sum0;
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
    x0_p = self->stateBfr_p + n - 1;
    x1_p = self->stateBfr_p + secondHalfIdx - 1;

    /* Init coeffs pointers */
    c0_p = self->coeffs_p;
    c1_p = self->coeffs_p + secondHalfIdx;

    /* Set tapCnt to blocks of 8 samples */
    tapCnt = n >> 3u;

    /* Process blocks of 8 samples */
    while (tapCnt > 0u)
    {
        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *c0_p++;
        c1 = *c1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

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

static inline void fir_turn(
    fir_t*          self
)
{
    volatile q15_t* x0_p;
    volatile q15_t* x1_p;
    uint32_t tapCnt;
    uint32_t n;

    /* Load n */
    n = self->length;

    /* Init state pointers */
    x0_p = self->stateBfr_p;
    x1_p = self->stateBfr_p + 1;

    /* Set tapCnt to blocks of 8 samples without last 7 sample block */
    tapCnt = n >> 3u - 1;

    /* Copy data blocks of 8 samples */
    while (tapCnt > 0u)
    {
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;

        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;

        /* Decrement tapCnt */
        tapCnt--;
    }

    /* Process last 7 sample block */
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;

    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
}

#endif /* FIR_H_ */
