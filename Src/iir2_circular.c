/*
 * iir2_circular.c
 *
 *  Created on: Nov 2, 2020
 *      Author: klukomski
 */

#include "iir2_circular.h"

#include <string.h>

/* Public methods definition */

void iir2_circular_init(
    iir2_circular_t*    self,
    volatile q15_t*     b_coeffs_p,
    volatile q15_t*     a_coeffs_p,
    const q31_t         scaling_factor,
    volatile q15_t*     oldDataIn_p,
    volatile q15_t*     oldDataOut_p
)
{
    self->b_coeffs_p     = b_coeffs_p;
    self->a_coeffs_p     = a_coeffs_p;
    self->scaling_factor = scaling_factor;
    self->x1_p           = oldDataIn_p;
    self->y1_p           = oldDataOut_p;
    self->x2             = 0;
    self->x0             = 0;
    self->y0             = 0;
}