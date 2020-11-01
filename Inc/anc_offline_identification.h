/*
 * anc_offline_identification.h
 *
 *  Created on: Sep 19, 2020
 *      Author: klukomski
 */

#ifndef ANC_OFFLINE_IDENTIFICATION_H_
#define ANC_OFFLINE_IDENTIFICATION_H_

#include "anc_math.h"

#include "anc_parameters.h"

#include "anc_processing.h"

#include "fir.h"
#include "lnlms.h"

volatile q15_t errFiltered;

#define ANC_OFFLINE_IDENTIFICATION_HALF     0u
#define ANC_OFFLINE_IDENTIFICATION_FULL     1u

struct anc_offline_identification_struct
{
    fir_t           fir_Sn;
    lnlms_t         lnlms;
    q15_t*          excitationSignal_p;
    q15_t           out;
    uint32_t        counter;
    uint32_t        identificationCycles;
    volatile q15_t* Sn_state_p;
};

typedef volatile struct anc_offline_identification_struct anc_offline_identification_t;


/* Public methods declaration */

void anc_offline_identification_init(
    anc_offline_identification_t*   self,
    uint32_t                        identificationCycles,
    volatile q15_t*                 Sn_state_p
);

__attribute__((weak)) void anc_offline_identification_onEndCallback(
    anc_offline_identification_t* self
);

__attribute__((weak)) void anc_offline_identification_onErrorCallback(
    anc_offline_identification_t* self
);

/* Inline methods */

static inline q15_t anc_offline_identification_calculate(
    anc_offline_identification_t*       self,
    anc_processing_preprocessing_data_t samples
)
{
    q31_t error;
    q15_t out;
    uint32_t counter = self->counter;

    if (counter < self->identificationCycles)
    {
        /* Load old out */
        out = self->out;

        /* Filter old out with Sn Path as reference */
        fir_pushData(&(self->fir_Sn), out);
        out = fir_calculate(&(self->fir_Sn));

        /* Calculate measured error with reference model */
        error = (q31_t) samples.errSample - (q31_t) out;

        /* Temporary only */
        errFiltered = (q15_t) __SSAT(error, 16);
        //errFiltered = out;

        /* Update Sn using LNLMS algorithm */
        lnlms_update(&self->lnlms, error);

        /* Shift states */
        fir_turn(&(self->fir_Sn));

        /* Increment counter */
        counter++;
        if (counter > self->identificationCycles)
        {
            anc_offline_identification_onEndCallback(self);
        }

        /* Generate new excitation output */
        out = self->excitationSignal_p[counter % ANC_LMS_EXCITATION_SIGNAL_LENGTH];

        /* Save out signal */
        self->out = out;

        /* Save counter */
        self->counter = counter;
    }
    else
    {
        /* No excitation signal when identification ends */
        out = 0;
    }

    return out;
}

#endif /* ANC_OFFLINE_IDENTIFICATION_H_ */
