/*
 * fir_decimate.h
 *
 *  Created on: Aug 14, 2020
 *      Author: klukomski
 */

#ifndef FIR_DECIMATE_H_
#define FIR_DECIMATE_H_

#include "ringbuffer.h"

#define FIR_DECIMATE_ORDER      31
#define FIR_DECIMATE_CHUNK_SIZE 4
#define FIR_DECIMATE_BFR_SIZE   (FIR_DECIMATE_ORDER + 1)
#define FIR_DECIMATE_CHUNK_NUM  (FIR_DECIMATE_BFR_SIZE / FIR_DECIMATE_CHUNK_SIZE)

struct fir_decimate_struct
{
    float*          coeffs_p;
    float           bfr_p[FIR_DECIMATE_BFR_SIZE];
    ringbuffer_t    h_ringbuffer;
};

typedef struct fir_decimate_struct fir_decimate_t;

void    fir_decimate_init(
    fir_decimate_t* self,
    float*          coeffs_p
);

void    fir_decimate_pushData(
    fir_decimate_t* self,
    float*          newData_f
);

float   fir_decimate_calculate(
    fir_decimate_t* self
);

float   fir_decimate_perform(
    fir_decimate_t* self,
    float*          newData_f
);

#endif /* FIR_DECIMATE_H_ */
