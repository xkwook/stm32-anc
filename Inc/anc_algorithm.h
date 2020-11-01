/*
 * anc_algorithm.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef ANC_ALGORITHM_H_
#define ANC_ALGORITHM_H_

#include "anc_math.h"

#include "anc_parameters.h"

#include "anc_processing.h"

#include "fir.h"
#include "lnlms.h"


struct anc_algorithm_struct
{
    fir_t       fir_Sn;
    fir_t       fir_SnOut;
    fir_t       fir_Wn;
    lnlms_t     lnlms;
    uint32_t    enable;
};

typedef volatile struct anc_algorithm_struct anc_algorithm_t;


/* Public methods declaration */

void anc_algorithm_init(
    anc_algorithm_t*        self,
    anc_algorithm_states_t* states_p
);

void anc_algorithm_enable(anc_algorithm_t* self);

void anc_algorithm_disable(anc_algorithm_t* self);

__attribute__((weak)) void anc_algorithm_onErrorCallback(
    anc_algorithm_t* self
);

/* Inline methods */

static inline q15_t anc_algorithm_calculate(
    anc_algorithm_t*                    self,
    anc_processing_preprocessing_data_t samples
)
{
    q15_t refSn;
    q15_t result;

    if (self->enable == ENABLED)
    {
        /* Filter Ref with Sn Path */
        fir_pushData(&(self->fir_Sn), samples.refSample);
        refSn = fir_calculate(&(self->fir_Sn));

        /* Append refSn as filter state (not using calculate from FIR filter) */
        fir_pushData(&(self->fir_SnOut), refSn);

        /* Update Wn using LNLMS algorithm */
        lnlms_update(&(self->lnlms), (q31_t)samples.errSample);

        /* Filter Ref with Wn Path and calculate u */
        fir_pushData(&(self->fir_Wn), samples.refSample);
        result = fir_calculate(&(self->fir_Wn));

        /* Shift all states */
        fir_turn(&(self->fir_Sn));
        fir_turn(&(self->fir_SnOut));
        fir_turn(&(self->fir_Wn));
    }
    else
    {
        /* No output waveform generated */
        result = 0;
    }

    return result;
}

#endif /* ANC_ALGORITHM_H_ */