/*
 * iir.c
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#include "iir.h"
#include <string.h>

void iir_init(
    iir_t*  self,
    float*  b_coeffs_p,
    float*  a_coeffs_p
)
{
    self->b_coeffs_p = b_coeffs_p;
    self->a_coeffs_p = a_coeffs_p;
    for (uint32_t i = 0; i < IIR_ORDER; i++)
    {
        self->dataIn_f[i]       = 0.0;
        self->dataOut_f[i]      = 0.0;
    }
}

void iir_pushData(
    iir_t*      self,
    int16_t*    newDataChunk_i16
)
{
    /* Put new data and cast it to floats */
    for (uint32_t i = 0; i < IIR_DATA_CHUNK; i++)
    {
        dataIn_f[IIR_ORDER + i] = (float)newDataChunk_i16[i];
    }
}

void iir_turn(
    iir_t*  self
)
{
    /* Copy last N (filter order) data to remember for calculation */
    memcpy(self->dataIn_f, (self->dataIn_f + IIR_DATA_CHUNK), IIR_ORDER*sizeof(float));
    memcpy(self->dataOut_f, (self->dataOut_f + IIR_DATA_CHUNK), IIR_ORDER*sizeof(float));
}

float* iir_calculate(
    iir_t*  self
)
{
    float* x = &self->dataIn_f[IIR_ORDER];
    float* y = &self->dataOut_f[IIR_ORDER];
    float* b = self->b_coeffs_p;
    float* a = self->a_coeffs_p;
    for (int32_t i = 0; i < IIR_DATA_CHUNK; i++)
    {
        y[i] = b[0] * x[0];
        for (int32_t k = 0; k < IIR_ORDER; k++)
        {
            y[i] += b[k + 1] * x[i - k - 1];
            y[i] -= a[k] * y[i - k - 1];
        }
    }
    return y;
}

float* iir_perform(
    iir_t*      self,
    int16_t*    newDataChunk_i16
)
{
    float* retVal_p;
    iir_pushData(self, newDataChunk_i16);
    retVal_p = iir_calculate(self);
    iir_turn(self);
    return retVal_p;
}