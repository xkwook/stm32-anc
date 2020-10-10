/*
 * iir3_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef IIR3_CIRCULAR_H_
#define IIR3_CIRCULAR_H_

#include "anc_math.h"

struct iir3_circular_struct {
    volatile q15_t* b_coeffs_p;
    volatile q15_t* a_coeffs_p;
    q31_t           scaling_factor;
    q15_t           x0;
    q15_t           x1;
    volatile q15_t* x2_p;
    q15_t           x3;
    q15_t           y0;
    q15_t           y1;
    volatile q15_t* y2_p;
};

typedef volatile struct iir3_circular_struct iir3_circular_t;

/* Public methods declaration */

void iir3_circular_init(
    iir3_circular_t*    self,
    volatile q15_t*     b_coeffs_p,
    volatile q15_t*     a_coeffs_p,
    const q31_t         scaling_factor,
    volatile q15_t*     oldDataIn_p,
    volatile q15_t*     oldDataOut_p
);

static inline void iir3_circular_pushData(
    iir3_circular_t*    self,
    q15_t               dataIn
)
{
    self->x3 = dataIn;
}

static inline volatile q15_t* iir3_circular_getDataInPtr(iir3_circular_t* self)
{
    return &(self->x3);
}

static inline volatile q15_t* iir3_circular_getDataOutPtr(iir3_circular_t* self)
{
    return &(self->y1);
}

static inline q15_t iir3_circular_calculate(iir3_circular_t* self)
{
    q15_t  x2   = *(self->x2_p);
    q15_t  y2   = *(self->y2_p);
    volatile q15_t* c0_p;
    volatile q15_t* c1_p;
    q15_t  c0, c1;
    q15_t  out;
    q63_t  acc0, acc1, sum0;

    acc0 = 0;
    acc1 = 0;

    /* Set coeffs for x */
    c0_p = self->b_coeffs_p;
    c1_p = self->b_coeffs_p + 2;

    /* Load coeffs */
    c0 = *c0_p++;
    c1 = *c1_p++;

    /* Perform multiply-accumulate */
    acc0 += self->x3 * c0;
    acc1 += self->x1 * c1;

    /* Load coeffs */
    c0 = *c0_p;
    c1 = *c1_p;

    /* Perform multiply-accumulate */
    acc0 += x2 * c0;
    acc1 += self->x0 * c1;

    /* Set coeffs for y */
    c0_p = self->a_coeffs_p;
    c1_p = self->a_coeffs_p + 2;

    /* Load coeffs */
    c0 = *c0_p++;
    c1 = *c1_p;

    /* Perform multiply-substract */
    acc0 -= y2 * c0;
    acc1 -= self->y0 * c1;

    /* Load last coeff */
    c0 = *c0_p;

    /* Last multiply-substract */
    acc0 -= self->y1 * c0;

    /* Sum results */
    sum0 = acc0 + acc1;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    out = (q15_t) (__SSAT((sum0 >> 15), 16));

    /* Multiply result by scaling factor */
    sum0 = ((q31_t) out) * self->scaling_factor;

    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    out = (q15_t) (__SSAT((sum0 >> 15), 16));

    /* Shift x */
    self->x0 = x2;
    self->x1 = self->x3;

    /* Now shift y with result */
    self->y0 = y2;
    self->y1 = out;

    return out;
}

#endif /* IIR3_CIRCULAR_H_ */
