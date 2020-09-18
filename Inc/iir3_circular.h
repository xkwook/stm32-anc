/*
 * iir3_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef IIR3_CIRCULAR_H_
#define IIR3_CIRCULAR_H_

#include "anc_math.h"

struct iir3_struct {
    q15_t*  b_coeffs_p;
    q15_t*  a_coeffs_p;
    q15_t   x0;
    q15_t   x1;
    q15_t*  x2_p;
    q15_t   x3;
    q15_t   y0;
    q15_t   y1;
    q15_t*  y2_p;
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

inline void iir3_circular_pushData(
    iir3_circular_t*    self,
    q15_t               dataIn
);

inline q15_t* iir3_circular_getOldDataInPtr(iir3_circular_t* self);

inline q15_t* iir3_circular_getOldDataOutPtr(iir3_circular_t* self);

inline q15_t iir3_circular_calculate(iir3_circular_t* self);

#endif /* IIR3_CIRCULAR_H_ */
