/*
 * lnlms.c
 *
 *  Created on: Nov 1, 2020
 *      Author: klukomski
 */

#include "lnlms.h"

/* Public methods definition */

void lnlms_init(
    lnlms_t*        self,
    state_buffer_t* stateBfr,
    volatile float* coeffs_p,
    float           alpha,
    float           mu
)
{
    self->stateBfr  = stateBfr;
    self->coeffs_p  = coeffs_p;
    self->alpha     = alpha;
    self->mu        = mu;

    /* Zero energy for summing */
    self->energy    = 0.0;
}

void lnlms_initCoeffs(
    volatile float*     coeffs_p,
    uint32_t            length
)
{
    /* Init coeffs vector to zeros */
    for (int i = 0; i < length; i++)
    {
        coeffs_p[i] = 0.0;
    }
}
