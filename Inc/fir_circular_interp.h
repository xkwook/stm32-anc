/*
 * fir_circular_interp.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef FIR_CIRCULAR_INTERP_H_
#define FIR_CIRCULAR_INTERP_H_

#include "anc_math.h"

#define FIR_CIRCULAR_INTERP_LENGTH        32
#define FIR_CIRCULAR_INTERP_CHUNK_SIZE    4
#define FIR_CIRCULAR_INTERP_CHUNK_NUM     \
    (FIR_CIRCULAR_INTERP_LENGTH / FIR_CIRCULAR_INTERP_CHUNK_SIZE)
#define FIR_CIRCULAR_INTERP_BFR_SIZE      \
    ((FIR_CIRCULAR_INTERP_CHUNK_NUM - 2) * FIR_CIRCULAR_INTERP_CHUNK_SIZE)

struct fir_circular_interp_struct
{
    q15_t*  coeffs_p;
    q15_t*  oldDataIn_p;
    q15_t*  dataIn_p;
    q15_t   stateBfr[FIR_CIRCULAR_INTERP_BFR_SIZE];
};

typedef struct fir_circular_interp_struct fir_circular_interp_t;

/* Public methods declaration */

void fir_circular_interp_init(
    fir_circular_interp_t*  self,
    q15_t*                      coeffs_p,
    q15_t*                      oldDataIn_p,
    q15_t*                      dataIn_p
);

inline q15_t fir_circular_interp_calculate(
    fir_circular_interp_t* self
);

#endif /* FIR_CIRCULAR_INTERP_H_ */
