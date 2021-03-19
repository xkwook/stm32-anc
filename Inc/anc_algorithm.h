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
    state_buffer_t  stateBfr_Sn;
    state_buffer_t  stateBfr_SnOut;
    state_buffer_t  stateBfr_Wn;
    fir_t           fir_Sn;
    fir_t           fir_Wn;
    lnlms_t         lnlms;
    uint32_t        enable;
};

typedef volatile struct anc_algorithm_struct anc_algorithm_t;


/* Public methods declaration */

void anc_algorithm_init(
    anc_algorithm_t*    self,
    float               mu,
    float               alpha
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
    float refSample, errSample;
    float refSn;
    float result_f;
    q15_t result;

    if (self->enable == ENABLED)
    {
        /* Convert samples to float */
        refSample = q15_to_float(samples.refSample);
        errSample = q15_to_float(samples.errSample);

        /* Filter Ref with Sn Path */
        state_buffer_pushData(&(self->stateBfr_Sn), refSample);
        refSn = fir_calculate(&(self->fir_Sn));

        /* Append refSn to state buffer */
        state_buffer_pushData(&(self->stateBfr_SnOut), refSn);

        /* Update Wn(n+1) using LNLMS algorithm */
        lnlms_update(&(self->lnlms), errSample);

        /* Filter Ref with Wn Path and calculate u(n+1) using w(n) */
        state_buffer_pushData(&(self->stateBfr_Wn), refSample);
        result_f = fir_calculate(&(self->fir_Wn));

        result = float_to_q15(result_f);

        /* Shift all states */
        state_buffer_turn(&(self->stateBfr_Sn));
        state_buffer_turn(&(self->stateBfr_SnOut));
        state_buffer_turn(&(self->stateBfr_Wn));
    }
    else
    {
        /* No output waveform generated */
        result = 0;
    }

    return result;
}

#endif /* ANC_ALGORITHM_H_ */
