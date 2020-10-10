/*
 * fir_circular_interp.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "fir_circular_interp.h"

#include <string.h>


/* Public methods definition */

void fir_circular_interp_init(
    fir_circular_interp_t*  self,
    volatile q15_t*         coeffs_p,
    volatile q15_t*         oldDataIn_p
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->dataIn        = 0;

    for (int i = 0; i < FIR_CIRCULAR_INTERP_BFR_SIZE; i++)
    {
        self->stateBfr[i] = 0;
    }
}
