/*
 * fir_circular_decimate.c
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "fir_circular_decimate.h"

#include <string.h>

/* Public methods definition */

void fir_circular_decimate_init(
    fir_circular_decimate_t*    self,
    q15_t*                      coeffs_p,
    q15_t*                      oldDataIn_p,
    q15_t*                      dataIn_p
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->dataIn_p      = dataIn_p;

    memset(self->stateBfr, 0, sizeof(self->stateBfr));
}

inline q15_t fir_circular_decimate_calculate(
    fir_circular_decimate_t* self
)
{
    q15_t* b0_p;
    q15_t* b1_p;
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t c0, c1;
    q15_t x0, x1;
    q31_t acc0, acc1, sum0;
    q15_t result;
    uint32_t i;

    /* Load coeffs pointers */
    b0_p = self->coeffs_p;
    b1_p = self->coeffs_p + FIR_CIRCULAR_DECIMATE_CHUNK_SIZE;

    /* Load state pointers from end */
    x0_p = self->dataIn_p + FIR_CIRCULAR_DECIMATE_CHUNK_SIZE - 1;
    x1_p = self->oldDataIn_p + FIR_CIRCULAR_DECIMATE_CHUNK_SIZE - 1;

    /* Zero accumulators */
    acc0 = 0;
    acc1 = 0;

    /* Load state variables */
    x0 = *x0_p--;
    x1 = *x1_p--;

    /* Load coeffs */
    c0 = *b0_p++;
    c1 = *b1_p++;

    /* Perform multiply-accumulate */
    acc0 += x0 * c0;
    acc1 += x1 * c1;

    /* Load state variables */
    x0 = *x0_p--;
    x1 = *x1_p--;

    /* Load coeffs */
    c0 = *b0_p++;
    c1 = *b1_p++;

    /* Perform multiply-accumulate */
    acc0 += x0 * c0;
    acc1 += x1 * c1;

    /* Load state variabes */
    x0 = *x0_p--;
    x1 = *x1_p--;

    /* Load coeffs */
    c0 = *b0_p++;
    c1 = *b1_p++;

    /* Perform multiply-accumulate */
    acc0 += x0 * c0;
    acc1 += x1 * c1;

    /* Load state variables */
    x0 = *x0_p--;
    x1 = *x1_p--;

    /* Load coeffs */
    c0 = *b0_p++;
    c1 = *b1_p++;

    /* Perform multiply-accumulate */
    acc0 += x0 * c0;
    acc1 += x1 * c1;

    /* Now we have all data calculated for new data and old data */

    /* Load new coeffs pointers */
    b0_p = self->coeffs_p + 3 * FIR_CIRCULAR_DECIMATE_CHUNK_SIZE;
    /* Pointer b1_p points to proper coeffs for state
     * at the beggining of the buffer, so there's no need to load it.
     */

    /* Load new state pointers from end */
    x0_p = self->stateBfr + FIR_CIRCULAR_DECIMATE_BFR_SIZE - FIR_CIRCULAR_DECIMATE_CHUNK_SIZE - 1;
    x1_p = self->stateBfr + FIR_CIRCULAR_DECIMATE_BFR_SIZE - 1;

    for (i = 0; i < (FIR_CIRCULAR_DECIMATE_CHUNK_NUM - 2); i++)
    {
        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *b0_p++;
        c1 = *b1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *b0_p++;
        c1 = *b1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *b0_p++;
        c1 = *b1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Load coeffs */
        c0 = *b0_p++;
        c1 = *b1_p++;

        /* Perform multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c1;
    }

    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    result = (q15_t) (__SSAT((sum0 >> 15), 16));

    /* Now move state buffer by decimation factor * 2 and add new data at the end */

    x0_p = self->stateBfr;
    x1_p = self->stateBfr + 2 * FIR_CIRCULAR_DECIMATE_CHUNK_SIZE;

    /* Copy data */
    for (i = 0; i < (FIR_CIRCULAR_DECIMATE_CHUNK_NUM - 4); i++)
    {
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
    }

    /* Copy now oldData to state buffer */
    x1_p = self->oldDataIn_p;

    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p;

    /* Copy now new Data to state buffer */
    x1_p = self->dataIn_p;

    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p++ = *x1_p++;
    *x0_p   = *x1_p;

    return result;
}