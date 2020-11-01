/*
 * anc_offline_identification.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_offline_identification.h"


/* Public methods definition */

void anc_offline_identification_init(
    anc_offline_identification_t*   self,
    uint32_t                        identificationCycles,
    volatile q15_t*                 Sn_state_p
)
{
    self->identificationCycles  = identificationCycles;
    self->counter               = 0;
    self->out                   = 0;
    self->excitationSignal_p    = (q15_t*) anc_lms_excitationSignal;

    /* Init filter */
    fir_init(
        &(self->fir_Sn),
        anc_Sn_coeffs,
        Sn_state_p,
        ANC_SN_FILTER_LENGTH
    );

    /* Init LNLMS algorithm */
    lnlms_init(
        &(self->lnlms),
        anc_Sn_coeffs,
        ANC_OFFLINE_ALPHA,
        ANC_OFFLINE_MU,
        Sn_state_p,
        ANC_SN_FILTER_LENGTH
    );
}
