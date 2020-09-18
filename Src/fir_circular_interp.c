/*
 * fir_circular_interp.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "fir_circular_interp.h"

#include <string.h>


/* Public methods definition */

void fir_circular_interp_init(
    fir_circular_interp_t*  self,
    q15_t*                  coeffs_p,
    q15_t*                  oldDataIn_p
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->dataIn        = 0;

    memset(self->stateBfr, 0, sizeof(self->stateBfr));
}

inline void fir_circular_interp_pushData(
    fir_circular_interp_t*  self,
    q15_t                   dataIn
)
{
    self->dataIn = dataIn;
}

inline q15_t* fir_circular_interp_getOldDataInPtr(
    fir_circular_interp_t*  self
)
{
    return &(self->dataIn);
}

inline q15_t fir_circular_interp_calculate(
    fir_circular_interp_t*  self,
    q15_t*                  dataOut_p
)
{
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t* c0_p;
    q15_t* c1_p;
    q15_t x0, x1;
    q15_t c0, c1;
    q31_t acc0, acc1, sum0;
    uint32_t i;

    /* Zero accumulators */
    acc0 = 0;
    acc1 = 0;

    /* Load first samples */
    x0 = self->dataIn;
    x1 = *(self->oldDataIn_p);

    /* Load first coeffs */
    c0 = *(self->coeffs_p);
    c1 = *(self->coeffs_p + FIR_CIRCULAR_INTERP_CHUNK_SIZE);

    /* Perform multiply-acumulate */
    acc0 += c0 * x0;
    acc1 += c1 * x1;

    /* Load state pointers */
    x0_p = self->stateBfr;
    x1_p = self->stateBfr + FIR_CIRCULAR_INTERP_BFR_SIZE / 2;

    /* Load coeffs pointers */
    c0_p = self->coeffs_p + FIR_CIRCULAR_INTERP_CHUNK_SIZE;
    c1_p = self->coeffs_p + (FIR_CIRCULAR_INTERP_BFR_SIZE / 2 + 1)
         * FIR_CIRCULAR_INTERP_CHUNK_SIZE;

    /* Load states */
    x0 = *x0_p++;
    x1 = *x1_p++;

    /* Load coeffs */
    c0_p
}

inline void fir_circular_interp_calculate(
    fir_circular_interp_t*  self,
    q15_t*                  dataOut_p
)
{
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t* c_p;
    q15_t x0, x0_old, x0_new;
    q15_t c0, c1, c2, c3;
    q31_t acc0, acc1, acc2, acc3;
    uint32_t i;

    /* Zero accumulators */
    acc0 = 0;
    acc1 = 0;
    acc2 = 0;
    acc3 = 0;

    /* Load last sample from state buffer */
    x0 = self->stateBfr[FIR_CIRCULAR_INTERP_BFR_SIZE - 1];
    x0_new = self->dataIn;

    /* Load first coeff for new data */
    c3 = self->coeffs_p[0];

    /* Load last coeffs for latest sample */
    c2 = self->coeffs_p[FIR_CIRCULAR_INTERP_LENGTH - 3];
    c1 = self->coeffs_p[FIR_CIRCULAR_INTERP_LENGTH - 2];
    c0 = self->coeffs_p[FIR_CIRCULAR_INTERP_LENGTH - 1];

    /* Perform multiply-acumulate */
    acc3 += c3 * x0_new;
    acc2 += c2 * x0;
    acc1 += c1 * x0;
    acc0 += c0 * x0;

    /* Load sample for old data */
    x0_old = *(self->oldDataIn_p);

    /* Load coeffs */
    c3 = self->coeffs_p[FIR_CIRCULAR_INTERP_CHUNK_SIZE];
    c2 = self->coeffs_p[FIR_CIRCULAR_INTERP_CHUNK_SIZE - 1];
    c1 = self->coeffs_p[FIR_CIRCULAR_INTERP_CHUNK_SIZE - 2];
    c0 = self->coeffs_p[FIR_CIRCULAR_INTERP_CHUNK_SIZE - 3];

    /* Perform multiply-accumulate */
    acc3 += c3 * x0_old;
    acc2 += c2 * x0_old;
    acc1 += c1 * x0_old;
    acc0 += c0 * x0_old;

    /* Load sample pointer to state buffer */
    x0_p = self->stateBfr;

    /* Load coeffs pointer for rest samples */
    c_p = self->coeffs_p + FIR_CIRCULAR_INTERP_CHUNK_SIZE;

    /* Calculation for rest of samples from state buffer - 1 */
    for (i = 0; i < (FIR_CIRCULAR_INTERP_BFR_SIZE - 2); i++)
    {
        /* Load sample */
        x0 = *x0_p++;

        /* Load coeffs */
        c3 = *(c_p);
        c2 = *(c_p - 1);
        c1 = *(c_p - 2);
        c0 = *(c_p - 3);

        /* Perform multiply accumulate */
        acc3 += c3 * x0;
        acc2 += c2 * x0;
        acc1 += c1 * x0;
        acc0 += c0 * x0;

        /* Move to next chunk coeffs */
        c_p += FIR_CIRCULAR_INTERP_CHUNK_SIZE;
    }

    /* Last data operations now, so no need to move to next coeffs/samples */

    /* Load sample */
    x0 = *x0_p;

    /* Load coeffs */
    c3 = *(c_p);
    c2 = *(c_p - 1);
    c1 = *(c_p - 2);
    c0 = *(c_p - 3);

    /* Perform multiply accumulate */
    acc3 += c3 * x0;
    acc2 += c2 * x0;
    acc1 += c1 * x0;
    acc0 += c0 * x0;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    dataOut_p[3] = (q15_t) (__SSAT((acc3 >> 15), 16));
    dataOut_p[2] = (q15_t) (__SSAT((acc2 >> 15), 16));
    dataOut_p[1] = (q15_t) (__SSAT((acc1 >> 15), 16));
    dataOut_p[0] = (q15_t) (__SSAT((acc0 >> 15), 16));

    /* Now move state buffer by 2 and add new data and old data at the end */

    x0_p = self->stateBfr + 2;
    x1_p = self->stateBfr;

    /* Copy data */
    for (i = 0; i < (FIR_CIRCULAR_INTERP_BFR_SIZE - 2); i++)
    {
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
        *x0_p++ = *x1_p++;
    }

    /* Copy now oldData to state buffer */
    self->stateBfr[1] = x0_old;

    /* Copy now newData to state buffer */
    self->stateBfr[0] = x0_new;
}