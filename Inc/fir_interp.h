/*
 * fir_interp.h
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#ifndef FIR_INTERP_H_
#define FIR_INTERP_H_

#include "ringbuffer.h"

#define FIR_INTERP_ORDER        31
#define FIR_INTERP_CHUNK_SIZE   4
#define FIR_INTERP_OP_PER_CHUNK ((FIR_INTERP_ORDER + 1) / FIR_INTERP_CHUNK_SIZE)
#define FIR_INTERP_BFR_SIZE     (FIR_INTERP_OP_PER_CHUNK)

struct fir_interp_struct
{
    float*          coeffs_p;
    float           bfr_p[FIR_INTERP_BFR_SIZE];
    float*          dataOut_f[FIR_INTERP_CHUNK_SIZE];
    ringbuffer_t    h_ringbuffer;
};

typedef struct fir_interp_struct fir_interp_t;

void    fir_interp_init(
    fir_interp_t*   self,
    float*          coeffs_p
)
{
    self->coeffs_p = coeffs_p;

    ringbuffer_init(
        self->h_ringbuffer,
        self->bfr_p,
        sizeof(self->bfr_p[0]),
        FIR_INTERP_BFR_SIZE
    );
}

float*  fir_interp_perform(
    fir_interp_t*   self,
    float           newData_f
)
{
    float   xi;
    float*  y = self->dataOut_f;
    float*  b = self->coeffs_p;

    for (uint32_t i = 0; i < (FIR_INTERP_CHUNK_SIZE - 1); i++)
    {
        y[i] = 0.0;
        for (uint32_t k = 0; k < FIR_INTERP_OP_PER_CHUNK; k++)
        {
            ringbuffer_getItem(self->h_ringbuffer, i, &xi);
            y[i] += b[i + 1 + FIR_INTERP_CHUNK_SIZE*k] * xi;
        }
    }
    /* Last element */
    y[FIR_INTERP_CHUNK_SIZE - 1] = b[0] * newData_f;
    for (uint32_t k = 1; k < FIR_INTERP_OP_PER_CHUNK; k++)
    {
        ringbuffer_getItem(self->h_ringbuffer, i, &xi);
        y[FIR_INTERP_CHUNK_SIZE - 1] = b[FIR_INTERP_CHUNK_SIZE*k] * xi;
    }

    ringbuffer_push(
        self->h_ringbuffer,
        &newData_f
    );

    return y;
}

#endif /* FIR_INTERP_H_ */