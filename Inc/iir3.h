/*
 * iir3.h
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#ifndef IIR3_H_
#define IIR3_H_

#include <stdint.h>

#define IIR3_DATA_CHUNK  4

/* Filter length must be no longer than data chunk size */

struct iir3_struct {
    float*  b_coeffs_p;
    float*  a_coeffs_p;
    float*  oldDataIn_p;
    float*  oldDataOut_p;
    float   dataIn_f[IIR3_DATA_CHUNK];
    float   dataOut_f[IIR3_DATA_CHUNK];
};

typedef struct iir3_struct iir3_t;

void iir3_init(
    iir_t*  self,
    float*  b_coeffs_p,
    float*  a_coeffs_p,
    float*  oldDataIn_p,
    float*  oldDataOut_p
)
{
    self->b_coeffs_p    = b_coeffs_p;
    self->a_coeffs_p    = a_coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->oldDataOut_p  = oldDataOut_p;
    for (uint32_t i = 0; i < IIR_DATA_CHUNK; i++)
    {
        self->dataIn_f[i]   = 0.0;
        self->dataOut_f[i]  = 0.0;
    }
}

void iir3_calculate(
    iir_t*      self,
    uint16_t*   newDataChunk_u16
)
{
    float* x  = self->dataIn_f;
    float* y  = self->dataOut_f;
    float* x_ = self->oldDataIn_p;
    float* y_ = self->oldDataOut_p;
    float* b  = self->b_coeffs_p;
    float* a  = self->a_coeffs_p;

    uint32_t END = (IIR_DATA_CHUNK - 1);

    for (uint32_t i = 0; i < IIR_DATA_CHUNK; i++)
    {
        x[i]   = (float)newDataChunk_u16[i];
    }

    y[0] = b[0] * x[0] + b[1] * x_[END] + b[2] * x_[END-1] + b[3] * x_[END-2]
         - a[0] * y_[END] - a[1] * y_[END-1] - a[2] * y_[END-2];
    y[1] = b[0] * x[1] + b[1] * x[0] + b[2] * x_[END] + b[3] * x_[END-1]
         - a[0] * y[0] - a[1] * y_[END] - a[2] * y_[END-1];
    y[2] = b[0] * x[2] + b[1] * x[1] + b[2] * x[0] + b[3] * x_[END]
         - a[0] * y[1] - a[1] * y[0] - a[2] * y_[END];
    y[3] = b[0] * x[3] + b[1] * x[2] + b[2] * x[1] + b[3] * x[0]
         - a[0] * y[2] - a[1] * y[1] - a[2] * y[0];
}

#endif /* IIR3_H_ */
