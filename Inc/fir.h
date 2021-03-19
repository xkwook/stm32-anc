/*
 * fir_circular.h
 *
 *  Created on: Nov 1, 2020
 *      Author: klukomski
 */

#ifndef FIR_H_
#define FIR_H_

#include "anc_math.h"
#include "state_buffer.h"

struct fir_struct
{
    state_buffer_t* stateBfr;
    volatile float* coeffs_p;
};

typedef volatile struct fir_struct fir_t;

/* Public methods declaration */

void fir_init(
    fir_t*          self,
    state_buffer_t* stateBfr,
    volatile float* coeffs_p
);

static inline float fir_calculate(
    fir_t* self
)
{
    volatile float* c_p;
    float  x;
    float  c;
    float  result;
    state_buffer_chunk_t chunk;

    /* Zero result accumulator */
    result = 0.0;

    /* Init coeffs pointer */
    c_p = self->coeffs_p;

    /* Load first chunk */
    chunk = state_buffer_firstChunk(self->stateBfr);

    while (chunk.length)
    {
        /* Load state variable */
        x = *chunk.ptr--;

        /* Load coeff */
        c = *c_p++;

        /* Perform multiply accumulate */
        result += c * x;

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
        c = *c_p++;

        /* Perform multiply accumulate */
        result += c * x;

        /* Decrement loop counter */
        chunk.length--;
    }

    return result;
}

#endif /* FIR_H_ */
