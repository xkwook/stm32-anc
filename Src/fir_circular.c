/*
 * fir_circular.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "fir_circular.h"

#include <string.h>

/* Public methods definiton */

void fir_circular_init(
    fir_circular_t* self,
    volatile q15_t* coeffs_p,
    volatile q15_t* oldDataIn_p,
    volatile q15_t* stateBfr_p,
    uint32_t        length
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->stateBfr_p    = stateBfr_p;
    self->length        = length;

    for (int i = 0; i < length; i++)
    {
        self->stateBfr_p[i] = 0;
    }
}

