/*
 * iir3_circular.c
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "iir3_circular.h"

#include <string.h>

/* Public methods definition */

void iir3_circular_init(
    iir3_circular_t*    self,
    q15_t*              b_coeffs_p,
    q15_t*              a_coeffs_p,
    q15_t*              oldDataIn_p,
    q15_t*              oldDataOut_p
)
{
    self->b_coeffs_p    = b_coeffs_p;
    self->a_coeffs_p    = a_coeffs_p;
    self->x2_p          = oldDataIn_p;
    self->y2_p          = oldDataOut_p;
    self->x3            = 0;
    self->x1            = 0;
    self->x0            = 0;
    self->y1            = 0;
    self->y0            = 0;
}

inline void iir3_circular_pushData(
    iir3_circular_t*    self,
    q15_t               dataIn
)
{
    self->x3 = dataIn;
}

inline q15_t* iir3_circular_getDataInPtr(iir3_circular_t* self)
{
    return &(self->x3);
}

inline q15_t* iir3_circular_getDataOutPtr(iir3_circular_t* self)
{
    return &(self->y1);
}

inline q15_t iir3_circular_calculate(iir3_circular_t* self)
{
    q15_t  x2   = *(self->x2_p);
    q15_t  y2   = *(self->y2_p);
    q15_t* c0_p = self->b_coeffs_p;
    q15_t* c1_p = self->b_coeffs_p + 2;
    q15_t  c0, c1;
    q31_t  acc0, acc1, sum0;

    acc0 = 0;
    acc1 = 0;

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

    /* Set new coeffs for y */
    c0_p = self->a_coeffs_p;

    /* Load coeffs */
    c0 = *c0_p++;
    c1 = *(c0_p + 2);

    /* Perform multiply-substract */
    acc0 -= y2 * c0;
    acc1 -= self->y0 * c1;

    /* Last multiply-substract */
    acc0 -= self->y1 * c0;

    /* Sum results */
    sum0 = acc0 + acc1;

    /* Now shift with calculation of result */
    self->y0 = y2;
    /* Results are stored as 2.14 format, so downscale by 15 to get output in 1.15 */
    self->y1 = (q15_t) (__SSAT((sum0 >> 15), 16));

    return self->y1;
}