/*
 * lnlms_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef LNLMS_CIRCULAR_H_
#define LNLMS_CIRCULAR_H_

#include "anc_math.h"

struct lnlms_circular_struct {
    q15_t*      coeffs_p;
    q15_t       alpha;
    float       mu_f;
    q15_t*      stateBfr_p;
    uint32_t    length;
    q31_t       energy;
};

typedef struct lnlms_circular_struct lnlms_circular_t;

/* Private methods declaration */

static inline q15_t lnmls_weightingFactor(q31_t energy, q31_t error, float mu);

/* Public methods declaration */

void lnlms_circular_init(
    lnlms_circular_t*   self,
    q15_t*              coeffs_p,
    q15_t               alpha,
    float               mu_f,
    q15_t*              stateBfr_p,
    uint32_t            length
);

void lnlms_circular_initWeights(
    lnlms_circular_t*   self
);

static inline void lnlms_circular_update(
    lnlms_circular_t*   self,
    q15_t               error
)
{
    q15_t* x0_p;
    q15_t* x1_p;
    q15_t* c0_p;
    q15_t* c1_p;
    q15_t  x0, x1;
    q31_t  c0, c1;
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
    energy = self->energy;
    /* Load alpha */
    alpha = self->alpha;

    secondHalfIdx = n >> 1u;

    /* Load new states */
    x0 = self->stateBfr_p[n - 1];
    x1 = self->stateBfr_p[n - 2];

    /* Calculate energy with new samples */
    acc0 = x0 * x0;
    acc1 = x1 * x1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    energy += ((acc0 + acc1) >> 15);

    /* Calculate weighting factor with the use of floats */
    w = lnmls_weightingFactor(energy, error, self->mu_f);

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

        /* Multiply by leakage factor */
        c0 = alpha * (*c0_p);
        c1 = alpha * (*c1_p);

        /* Perform multiply-accumulate */
        c0 += w * x0;
        c1 += w * x1;

        /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
        *c0_p++ = (q15_t) __SSAT((c0 >> 15), 16);
        *c1_p++ = (q15_t) __SSAT((c1 >> 15), 16);

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Multiply by leakage factor */
        c0 = alpha * (*c0_p);
        c1 = alpha * (*c1_p);

        /* Perform multiply-accumulate */
        c0 += w * x0;
        c1 += w * x1;

        /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
        *c0_p++ = (q15_t) __SSAT((c0 >> 15), 16);
        *c1_p++ = (q15_t) __SSAT((c1 >> 15), 16);

        /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

        /* Multiply by leakage factor */
        c0 = alpha * (*c0_p);
        c1 = alpha * (*c1_p);

        /* Perform multiply-accumulate */
        c0 += w * x0;
        c1 += w * x1;

        /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
        *c0_p++ = (q15_t) __SSAT((c0 >> 15), 16);
        *c1_p++ = (q15_t) __SSAT((c1 >> 15), 16);

            /* Load state variables */
        x0 = *x0_p--;
        x1 = *x1_p--;

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
    x0 = self->stateBfr_p[0];
    x1 = self->stateBfr_p[1];

    /* Remove energy from old samples for future */
    acc0 = x0 * x0;
    acc1 = x1 * x1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    energy -= ((acc0 + acc1) >> 15);

    /* Save energy */
    self->energy = energy;
}

/* Private methods definition */

static inline q15_t lnmls_weightingFactor(q31_t energy, q31_t error, float mu)
{
    static const float eps = 0.1 * (float) (1u << 15);
    float w_f;
    q15_t w;
    q31_t w_q31;

    /* Calculated result in float is in <-1; 1> format */
    w_f = ((float) error * mu) / ((float) energy + eps);

    /* Convert weighting factor to 1.15 format */
    w_q31 = (q31_t) (w_f * (float) (1u << 15));

    /* Saturate q31 weighting factor */
    w = (q15_t) (__SSAT(w_q31, 16));

    return w;
}

#endif /* LNLMS_CIRCULAR_H_ */
