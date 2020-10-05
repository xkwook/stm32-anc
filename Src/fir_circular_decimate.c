/*
 * fir_circular_decimate.c
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "fir_circular_decimate.h"

#include <string.h>

/* Public methods definition */

void fir_circular_decimate_init(
    fir_circular_decimate_t*    self,
    q15_t*                      coeffs_p,
    q15_t*                      oldDataIn_p,
    q15_t*                      dataIn_p
)
{
    self->coeffs_p      = coeffs_p;
    self->oldDataIn_p   = oldDataIn_p;
    self->dataIn_p      = dataIn_p;

    memset(self->stateBfr, 0, sizeof(self->stateBfr));
}