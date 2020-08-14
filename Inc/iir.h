/*
 * iir.h
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#ifndef IIR_H_
#define IIR_H_

#define IIR_ORDER       3
#define IIR_DATA_CHUNK  4

/* Filter length must be no longer than data chunk size */

struct iir_struct {
    float*  b_coeffs_p;
    float*  a_coeffs_p;
    float   dataIn_f[IIR_ORDER + IIR_DATA_CHUNK];
    float   dataOut_f[IIR_ORDER + IIR_DATA_CHUNK];
};

typedef struct iir_struct iir_t;

void iir_init(
    iir_t*  self,
    float*  b_coeffs_p,
    float*  a_coeffs_p
);

void iir_pushData(
    iir_t*      self,
    int16_t*    newDataChunk_i16
);

void iir_turn(
    iir_t*  self
);

float* iir_calculate(
    iir_t*  self
);

float* iir_perform(
    iir_t*      self,
    int16_t*    newDataChunk_i16
);

#endif /* IIR_H_ */
