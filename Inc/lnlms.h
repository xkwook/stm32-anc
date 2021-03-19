/*
 * lnlms.h
 *
 *  Created on: Nov 1, 2020
 *      Author: klukomski
 */

#ifndef LNLMS_H_
#define LNLMS_H_

#include "anc_math.h"
#include "state_buffer.h"
#include <stdio.h>

struct lnlms_struct {
    state_buffer_t* stateBfr;
    volatile float* coeffs_p;
    float           alpha;
    float           mu;
    float           energy;
};

typedef volatile struct lnlms_struct lnlms_t;

/* Private methods declaration */

static inline float lnmls_weightingFactor(float energy, float error, float mu);

/* Public methods declaration */

void lnlms_init(
    lnlms_t*        self,
    state_buffer_t* stateBfr,
    volatile float* coeffs_p,
    float           alpha,
    float           mu
);

void lnlms_initCoeffs(
    volatile float*     coeffs_p,
    uint32_t            length
);

static inline void lnlms_update(
    lnlms_t*            self,
    float               error
)
{
    volatile float* c_p;
    float x;
    float c;
    float w;
    float alpha;
    float mu;
    float energy;
    const float energy_leak = 0.999999;
    state_buffer_chunk_t chunk;

    /* Load alpha, mu and energy */
    alpha   = self->alpha;
    mu      = self->mu;
    energy  = self->energy;

    /* Init coeffs pointer */
    c_p     = self->coeffs_p;

    /* Load first chunk */
    chunk = state_buffer_firstChunk(self->stateBfr);

    /* Load newest sample */
    x = *chunk.ptr;

    /* Add newest sample to energy */
    energy  += x * x;

    /* Calculate weighting factor */

    w = lnmls_weightingFactor(energy, error, mu);

    while (chunk.length)
    {
        /* Load state variable */
        x = *chunk.ptr--;

        /* Load coeff */
        c = *c_p;

        /* Update coeff according to Leaky-LMS equation */
        *c_p++ = alpha * c + w * x;

        /* Decrement loop counter */
        chunk.length--;
    }

    /* Load second chunk */
    chunk = state_buffer_secondChunk(self->stateBfr);

    while (chunk.length)
    {
        /* Load state variable */
        x = *chunk.ptr--;

        /* Load coeff */
        c = *c_p;

        /* Update coeff according to Leaky-LMS equation */
        *c_p++ = alpha * c + w * x;

        /* Decrement loop counter */
        chunk.length--;
    }

    /* Remove latest loaded sample from energy */
    energy -= x * x;

    /* Save energy */
    //self->energy = energy_leak * energy;
    self->energy = energy;
}

/* Private methods definition */

static inline float lnmls_weightingFactor(float energy, float error, float mu)
{
    static const float eps = 0.1;
    float w;

    w = mu * error / (energy + eps);

    return w;
}

#endif /* LNLMS_H_ */
