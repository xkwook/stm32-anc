/*
 * anc_algorithm.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_algorithm.h"

/* Public methods definition */

void anc_algorithm_init(
    anc_algorithm_t*    self,
    float               mu,
    float               alpha
)
{
    self->enable    = DISABLED;

    /* Init state buffers */
    state_buffer_init(
        &(self->stateBfr_Sn),
        anc_Sn_bfr,
        ANC_SN_FILTER_LENGTH
    );

    state_buffer_init(
        &(self->stateBfr_SnOut),
        anc_SnOut_bfr,
        ANC_WN_FILTER_LENGTH
    );

    state_buffer_init(
        &(self->stateBfr_Wn),
        anc_Wn_bfr,
        ANC_WN_FILTER_LENGTH
    );

    /* Init filters */
    fir_init(
        &(self->fir_Sn),
        &(self->stateBfr_Sn),
        anc_Sn_coeffs
    );

    fir_init(
        &(self->fir_Wn),
        &(self->stateBfr_Wn),
        anc_Wn_coeffs
    );

    /* Init LNLMS algorithm */
    lnlms_init(
        &(self->lnlms),
        &(self->stateBfr_SnOut),
        anc_Wn_coeffs,
        mu,
        alpha
    );
}

void anc_algorithm_enable(anc_algorithm_t* self)
{
    if (self->enable == DISABLED)
    {
        self->enable = ENABLED;
    }
}

void anc_algorithm_disable(anc_algorithm_t* self)
{
    self->enable = DISABLED;
}
