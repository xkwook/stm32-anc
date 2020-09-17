/*
 * iir3_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef IIR3_CIRCULAR_H_
#define IIR3_CIRCULAR_H_

#include "anc_math.h"

#define IIR3_CIRCULAR_MIN_CHUNK     3
#define IIR3_CIRCULAR_DATA_CHUNK    4

struct iir3_struct {
    q15_t*  b_coeffs_p;
    q15_t*  a_coeffs_p;
    q15_t*  oldDataIn_p;
    q15_t*  oldDataOut_p;
    q15_t   dataIn  [IIR3_CIRCULAR_DATA_CHUNK];
    q15_t   dataOut [IIR3_CIRCULAR_DATA_CHUNK];
};

typedef struct iir3_struct iir3_circular_t;

/* Public methods definition */

void iir3_circular_init(
    iir3_circular_t*    self,
    q15_t*              b_coeffs_p,
    q15_t*              a_coeffs_p,
    q15_t*              oldDataIn_p,
    q15_t*              oldDataOut_p
);

inline q15_t* iir3_circular_getDataInPtr(iir3_circular_t* self);

inline q15_t* iir3_circular_getDataOutPtr(iir3_circular_t* self);

void iir3_circular_calculate(iir3_circular_t* self);

#endif /* IIR3_CIRCULAR_H_ */
