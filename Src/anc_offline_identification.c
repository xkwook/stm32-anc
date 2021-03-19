/*
 * anc_offline_identification.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_offline_identification.h"


#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#include "math.h" // for RAND, and rand
#include <stdlib.h>

double sampleNormal() {
    double u = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double v = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1) return sampleNormal();
    double c = sqrt(-2 * log(r) / r);
    return u * c;
}
#endif

/* Public methods definition */

void anc_offline_identification_init(
    anc_offline_identification_t*   self,
    uint32_t                        identificationCycles,
    float                           mu,
    float                           alpha
)
{
    self->identificationCycles  = identificationCycles;
    self->counter               = 0;
    self->out                   = 0.0;
    self->excitationSignal_p    = (q15_t*) anc_lms_excitationSignal;

    /* Init state buffer */
    state_buffer_init(
        &(self->stateBfr_Sn),
        anc_Sn_bfr,
        ANC_SN_FILTER_LENGTH
    );

    /* Init filter */
    fir_init(
        &(self->fir_Sn),
        &(self->stateBfr_Sn),
        anc_Sn_coeffs
    );

    /* Init LNLMS algorithm */
    lnlms_init(
        &(self->lnlms),
        &(self->stateBfr_Sn),
        anc_Sn_coeffs,
        alpha,
        mu
    );
}
