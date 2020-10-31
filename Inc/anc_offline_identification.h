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

#include "fir_circular.h"
#include "lnlms_circular.h"
#include "dma_mem2mem.h"

volatile q15_t errFiltered;

#define ANC_OFFLINE_IDENTIFICATION_HALF     0u
#define ANC_OFFLINE_IDENTIFICATION_FULL     1u

typedef volatile struct __attribute__(( packed, aligned(sizeof(uint32_t)) ))
{
    q15_t   Sn_state    [ANC_SN_FILTER_LENGTH];
} anc_offline_identification_states_t;

struct anc_offline_identification_struct
{
    dma_mem2mem_t*                      h_dma_mem2mem;
    fir_circular_t                      fir_Sn;
    lnlms_circular_t                    lnlms;
    q15_t*                              excitationSignal_p;
    q15_t                               out;
    volatile q15_t*                     oldOut_p;
    uint32_t                            counter;
    uint32_t                            identificationCycles;
    anc_offline_identification_states_t states;
};

typedef volatile struct anc_offline_identification_struct anc_offline_identification_t;


/* Public methods declaration */

void anc_offline_identification_init(
    anc_offline_identification_t*   self0,
    anc_offline_identification_t*   self1,
    dma_mem2mem_t*                  h_dma_mem2mem0,
    dma_mem2mem_t*                  h_dma_mem2mem1,
    uint32_t                        identificationCycles
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
        out = *(self->oldOut_p);

        /* Check if DMA mem2mem shifting is done */
        if (dma_mem2mem_isBusy(self->h_dma_mem2mem))
        {
            anc_offline_identification_onErrorCallback(self);
        }

        /* Filter old out with Sn Path as reference */
        fir_circular_pushData(&(self->fir_Sn), out);
        out = fir_circular_calculate(&(self->fir_Sn));

        /* Calculate measured error with reference model */
        error = (q31_t) samples.errSample - (q31_t) out;

        /* Temporary only */
        //errFiltered = (q15_t) __SSAT(error, 16);
        errFiltered = out;

        /* Update Sn using LNLMS algorithm */
        lnlms_circular_update(&self->lnlms, error);

        /* Run DMA mem2mem for shifting states */
        dma_mem2mem_start(self->h_dma_mem2mem);

        /* Increment counter */
        counter += 2u;
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
