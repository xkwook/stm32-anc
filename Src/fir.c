/*
 * fir.c
 *
 *  Created on: Nov 1, 2020
 *      Author: klukomski
 */

#include "fir.h"

/* Public methods definiton */

void fir_init(
    fir_t*          self,
    volatile q15_t* coeffs_p,
    volatile q15_t* stateBfr_p,
    uint32_t        length
)
{
    self->coeffs_p      = coeffs_p;
    self->stateBfr_p    = stateBfr_p;
    self->length        = length;

    for (int i = 0; i < length; i++)
    {
        self->stateBfr_p[i] = 0;
    }
}
