/*
 * fir_decimate.c
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#include "fir_decimate.h"

void    fir_decimate_init(
    fir_decimate_t* self,
    float*          coeffs_p
)
{
    self->coeffs_p = coeffs_p;

    ringbuffer_init(
        self->h_ringbuffer,
        self->bfr_p,
        FIR_DECIMATE_CHUNK_SIZE * sizeof(self->bfr_p[0]),
        FIR_DECIMATE_CHUNK_NUM
    );
}

void    fir_decimate_pushData(
    fir_decimate_t* self,
    float*          newData_f
)
{
    ringbuffer_push(
        self->h_ringbuffer,
        newData_f
    );
}

float   fir_decimate_calculate(
    fir_decimate_t* self
)
{
    float   retVal_f = 0.0;
    float*  b = self->coeffs_p;

    for (uint32_t i = 0; i < FIR_DECIMATE_CHUNK_NUM; i++)
    {
        float* chunk_f = (float*)ringbuffer_getItemPtr(
            self->h_ringbuffer, i);

        for (uint32_t k = 0; k < FIR_DECIMATE_CHUNK_SIZE; k++)
        {
            /* Items in chunk are reversed */
            retVal_f    += b[i * FIR_DECIMATE_CHUNK_SIZE + k]
                        *  chunk_f[FIR_DECIMATE_CHUNK_SIZE -1 - k];
        }
    }

    return retVal_f;
}

float   fir_decimate_perform(
    fir_decimate_t* self,
    float*          newData_f
)
{
    float retVal_f;
    fir_decimate_pushData(self, newData_f);
    retVal_f = fir_decimate_calculate(self);
    return retVal_f;
}