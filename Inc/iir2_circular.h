/*
 * iir2_circular.h
 *
 *  Created on: Nov 2, 2020
 *      Author: klukomski
 */

#ifndef IIR2_CIRCULAR_H_
#define IIR2_CIRCULAR_H_

#include "anc_math.h"

struct iir2_circular_struct {
    volatile q15_t* b_coeffs_p;
    volatile q15_t* a_coeffs_p;
    q31_t           scaling_factor;
    q15_t           x0;
    volatile q15_t* x1_p;
    q15_t           x2;
    q15_t           y0;
    volatile q15_t* y1_p;
};

typedef volatile struct iir2_circular_struct iir2_circular_t;

/* Public methods declaration */

void iir2_circular_init(
    iir2_circular_t*    self,
    volatile q15_t*     b_coeffs_p,
    volatile q15_t*     a_coeffs_p,
    const q31_t         scaling_factor,
    volatile q15_t*     oldDataIn_p,
    volatile q15_t*     oldDataOut_p
);

static inline void iir2_circular_pushData(
    iir2_circular_t*    self,
    q15_t               dataIn
)
{
    self->x2 = dataIn;
}

static inline volatile q15_t* iir2_circular_getDataInPtr(iir2_circular_t* self)
{
    return &(self->x2);
}

static inline volatile q15_t* iir2_circular_getDataOutPtr(iir2_circular_t* self)
{
    return &(self->y0);
}

static inline q15_t iir2_circular_calculate(iir2_circular_t* self)
{
    q15_t  x1   = *(self->x1_p);
    q15_t  y1   = *(self->y1_p);
    q15_t  c0, c1;
    q15_t  out;
    q63_t  acc0, acc1, sum0;

    /* Zero accumulators */
    acc0 = 0;
    acc1 = 0;

    /* Load coeffs for x2, x1 */
    c0 = self->b_coeffs_p[0];
    c1 = self->b_coeffs_p[1];

    /* Perform multiply-accumulate */
    acc0 += self->x2 * c0;
    acc1 += x1 * c1;

    /* Load coeffs for x0, y1 */
    c0 = self->b_coeffs_p[2];
    c1 = self->a_coeffs_p[0];

    /* Perform multiply-accumulate */
    acc0 += self->x0 * c0;
    acc1 -= y1 * c1;

    /* Load last coeff for y0 */
    c0 = self->a_coeffs_p[1];

    /* Perform last multiply-accumulate */
    acc0 -= self->y0 * c0;

    /* Sum results */
    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    out = (q15_t) (__SSAT((sum0 >> 15), 16));

    /* Multiply result by scaling factor */
    sum0 = ((q31_t) out) * self->scaling_factor;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    out = (q15_t) (__SSAT((sum0 >> 15), 16));

    /* Shift x */
    self->x0 = self->x2;

    /* Now shift y with result */
    self->y0 = out;

    return out;
}

#endif /* IIR2_CIRCULAR_H_ */
