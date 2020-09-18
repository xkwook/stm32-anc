/*
 * fir_circular.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef FIR_CIRCULAR_H_
#define FIR_CIRCULAR_H_

#include "anc_math.h"

struct fir_circular_struct
{
    q15_t*      coeffs_p;
    q15_t*      oldDataIn_p;
    q15_t       dataIn;
    q15_t*      stateBfr_p;
    uint32_t    length;
};

typedef struct fir_circular_struct fir_circular_t;

/* Public methods declaration */

void fir_circular_init(
    fir_circular_t* self,
    q15_t*          coeffs_p,
    q15_t*          oldDataIn_p,
    q15_t*          stateBfr_p,
    uint32_t        length
);

inline void fir_circular_pushData(
    fir_circular_t* self,
    q15_t           dataIn
);

inline q15_t* fir_circular_getDataInPtr(
    fir_circular_t* self
);

inline q15_t fir_circular_calculate(
    fir_circular_t* self
);

#endif /* FIR_CIRCULAR_H_ */
