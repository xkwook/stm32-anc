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
    self->x2_p           = oldDataIn_p;
    self->y2_p           = oldDataOut_p;
    self->x3             = 0;
    self->x1             = 0;
    self->x0             = 0;
    self->y1             = 0;
    self->y0             = 0;
}