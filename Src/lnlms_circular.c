/*
 * lnlms_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "lnlms_circular.h"

#include <string.h>

/* Private methods declaration */

static inline q15_t weightingFactor(q15_t energy, q31_t error, float mu);

/* Public methods definition */

void lnlms_circular_init(
    lnlms_circular_t*   self,
    q15_t*              coeffs_p,
    q15_t               alpha,
    float               mu_f,
    uint32_t            length
)
{
    self->coeffs_p  = coeffs_p;
    self->alpha     = alpha;
    self->mu_f      = mu_f;
    self->length    = length;

    /* Init coeffs vector to [1, 0, 0, 0, ...] */
    self->coeffs_p[0]   = (q15_t) (1u << 15 - 1);
    memset(self->coeffs_p + 1, 0, (self->length - 1) * sizeof(self->coeffs_p[0]));

    /* Zero energy for summing */
    self->energy    = 0;
}

inline void lnlms_circular_update(
    lnlms_circular_t*   self,
    q15_t               error
)
{
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t* c0_p;
    q15_t* c1_p;
    q15_t  x0, x1;
    q15_t  c0, c1;
    q31_t  acc0, acc1;
    q15_t  alpha;
    q31_t  energy;
    q15_t  w;
    uint32_t tapCnt;
    uint32_t secondHalfIdx;
    uint32_t n;

    /* Load length */
    n = self->length;
    /* Load energy */
    energy = (q31_t) self->energy;
    /* Load alpha */
    alpha = self->alpha;

    secondHalfIdx = n >> 1u;

    /* Load new states */
    x0 = self->stateBfr_p[0];
    x1 = self->stateBfr_p[1];

    /* Calculate energy with new samples */
    acc0 = x0 * x0;
    acc1 = x1 * x1;

    energy += acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    energy = (__SSAT((energy >> 15), 16));

    /* Calculate weighting factor with the use of floats */
    w = weightingFactor((q15_t) energy, error, self->mu_f);

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

        /* Multiply by leakage factor */
        c0 = alpha * (*c0_p);
        c1 = alpha * (*c1_p);

        /* Perform multiply-accumulate */
        c0 += w * x0;
        c1 += w * x1;

        /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
        *c0_p++ = (q15_t) __SSAT((c0 >> 15), 16);
        *c1_p++ = (q15_t) __SSAT((c1 >> 15), 16);

        /* Decrement tapCnt */
        tapCnt--;
    }

    /* Load states from end of buffer */
    x0 = self->stateBfr_p[n - 1];
    x1 = self->stateBfr_p[n - 2];

    /* Remove energy from old samples for future */
    acc0 = x0 * x0;
    acc1 = x1 * x1;

    energy -= (acc0 + acc1);

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    self->energy = (q15_t) (__SSAT((energy >> 15), 16));
}

/* Private methods definition */

static inline q15_t weightingFactor(q15_t energy, q31_t error, float mu)
{
    static const float eps = 0.1 * (float) (1u << 15);
    float w_f;
    q15_t w;

    /* Calculated result in float is in <-1; 1> format */
    w_f = ((float) error * mu) / ((float) energy + eps);

    /* Convert weighting factor to 1.15 format */
    w = (q15_t) (w_f * (float) (1u << 15));
}