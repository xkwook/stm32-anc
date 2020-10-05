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
    q15_t*                  coeffs_p,
    q15_t*                  oldDataIn_p
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->dataIn        = 0;

    memset(self->stateBfr, 0, sizeof(self->stateBfr));
}
