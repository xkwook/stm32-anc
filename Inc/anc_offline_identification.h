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

#if defined(ANC_MATH_TESTING_IMPLEMENTATION)

double sampleNormal();

#endif

volatile q15_t errFiltered;
volatile q15_t errFiltered2;

#define ANC_OFFLINE_IDENTIFICATION_HALF     0u
#define ANC_OFFLINE_IDENTIFICATION_FULL     1u

struct anc_offline_identification_struct
{
    state_buffer_t  stateBfr_Sn;
    fir_t           fir_Sn;
    lnlms_t         lnlms;
    q15_t*          excitationSignal_p;
    float           out;
    uint32_t        counter;
    uint32_t        identificationCycles;
};

typedef volatile struct anc_offline_identification_struct anc_offline_identification_t;


/* Public methods declaration */

void anc_offline_identification_init(
    anc_offline_identification_t*   self,
    uint32_t                        identificationCycles,
    float                           mu,
    float                           alpha
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
    q15_t out;
    float errSample;
    float error;
    float out_f;
    uint32_t counter = self->counter;

    if (counter < self->identificationCycles)
    {
        /* Convert sample q15 to float */
        errSample = q15_to_float(samples.errSample);

        /* Filter current out with Sn Path as reference */
        out_f = fir_calculate(&(self->fir_Sn));

        /* Calculate measured error with reference model */
        error = errSample - out_f;

        /* Temporary only */
        errFiltered = float_to_q15(error);
        errFiltered2 = float_to_q15(out_f);

        /* Update Sn using LNLMS algorithm */
        lnlms_update(&(self->lnlms), error);

        /* Shift state buffer */
        state_buffer_turn(&(self->stateBfr_Sn));

        /* Increment counter */
        counter++;
        if (counter == self->identificationCycles)
        {
            anc_offline_identification_onEndCallback(self);
        }

#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
        out = float_to_q15(0.25 * sampleNormal());
#else
        /* Generate new excitation output */
        out = self->excitationSignal_p[counter % ANC_LMS_EXCITATION_SIGNAL_LENGTH];
#endif

        /* Save new out signal to state buffer */
        out_f = q15_to_float(out);
        state_buffer_pushData(&(self->stateBfr_Sn), out_f);

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
