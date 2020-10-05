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

#include "fir_circular.h"
#include "lnlms_circular.h"
#include "dma_mem2mem.h"


typedef struct __attribute__(( packed, aligned(sizeof(uint32_t)) ))
{
    q15_t   Sn_state    [ANC_SN_FILTER_LENGTH];
    q15_t   SnOut_state [ANC_WN_FILTER_LENGTH];
    q15_t   Wn_state    [ANC_WN_FILTER_LENGTH];
} anc_algorithm_states_t;

struct anc_algorithm_struct
{
    dma_mem2mem_t*          h_dma_mem2mem;
    fir_circular_t          fir_Sn;
    fir_circular_t          fir_SnOut;
    fir_circular_t          fir_Wn;
    lnlms_circular_t        lnlms;
    uint32_t                enable;
    anc_algorithm_states_t  states;
};

typedef struct anc_algorithm_struct anc_algorithm_t;


/* Public methods declaration */

void anc_algorithm_init(
    anc_algorithm_t*    self0,
    anc_algorithm_t*    self1,
    dma_mem2mem_t*      h_dma_mem2mem0,
    dma_mem2mem_t*      h_dma_mem2mem1
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
        /* Check if DMA mem2mem shifting is done */
        if (dma_mem2mem_isBusy(self->h_dma_mem2mem))
        {
            anc_algorithm_onErrorCallback(self);
        }

        /* Filter Ref with Sn Path */
        fir_circular_pushData(&(self->fir_Sn), samples.refSample);
        refSn = fir_circular_calculate(&(self->fir_Sn));

        /* Append refSn as filter state (not using calculate from FIR filter) */
        fir_circular_pushData(&(self->fir_SnOut), refSn);

        /* Update Wn using LNLMS algorithm */
        lnlms_circular_update(&(self->lnlms), (q31_t)samples.errSample);

        /* Filter Ref with Wn Path and calculate u */
        fir_circular_pushData(&(self->fir_Wn), samples.refSample);
        result = fir_circular_calculate(&(self->fir_Wn));

        /* Run DMA mem2mem for shifting states */
        dma_mem2mem_start(self->h_dma_mem2mem);
    }
    else
    {
        /* No output waveform generated */
        result = 0;
    }

    return result;
}

#endif /* ANC_ALGORITHM_H_ */