/*
 * anc_algorithm.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_algorithm.h"

/* Public methods definition */

void anc_algorithm_init(
    anc_algorithm_t*        self,
    anc_algorithm_states_t* states_p
)
{
    self->enable    = DISABLED;

    /* Init filters */
    fir_init(
        &(self->fir_Sn),
        anc_Sn_coeffs,
        states_p->Sn_state,
        ANC_SN_FILTER_LENGTH
    );

    fir_init(
        &(self->fir_SnOut),
        NULL,
        states_p->SnOut_state,
        ANC_WN_FILTER_LENGTH
    );

    fir_init(
        &(self->fir_Wn),
        anc_Wn_coeffs,
        states_p->Wn_state,
        ANC_WN_FILTER_LENGTH
    );

    /* Init LNLMS algorithm */
    lnlms_init(
        &(self->lnlms),
        anc_Wn_coeffs,
        ANC_ONLINE_ALPHA,
        ANC_ONLINE_MU,
        states_p->SnOut_state,
        ANC_WN_FILTER_LENGTH
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
