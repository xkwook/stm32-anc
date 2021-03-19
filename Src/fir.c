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
    state_buffer_t* stateBfr,
    volatile float* coeffs_p
)
{
    self->coeffs_p  = coeffs_p;
    self->stateBfr  = stateBfr;
}
