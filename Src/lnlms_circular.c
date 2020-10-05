/*
 * lnlms_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#include "lnlms_circular.h"

#include <string.h>

/* Public methods definition */

void lnlms_circular_init(
    lnlms_circular_t*   self,
    q15_t*              coeffs_p,
    q15_t               alpha,
    float               mu_f,
    q15_t*              stateBfr_p,
    uint32_t            length
)
{
    self->coeffs_p      = coeffs_p;
    self->alpha         = alpha;
    self->mu_f          = mu_f;
    self->stateBfr_p    = stateBfr_p;
    self->length        = length;

    /* Zero energy for summing */
    self->energy    = 0;
}

void lnlms_circular_initWeights(
    lnlms_circular_t*   self
)
{
    /* Init coeffs vector to zeros */
    memset(self->coeffs_p, 0, self->length * sizeof(self->coeffs_p[0]));
}