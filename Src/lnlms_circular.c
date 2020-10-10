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
    volatile q15_t*     coeffs_p,
    q15_t               alpha,
    float               mu_f,
    volatile q15_t*     stateBfr_p,
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

void lnlms_circular_initCoeffs(
    volatile q15_t*     coeffs_p,
    uint32_t            length
)
{
    /* Init coeffs vector to zeros */
    for (int i = 0; i < length; i++)
    {
        coeffs_p[i] = 0;
    }
}