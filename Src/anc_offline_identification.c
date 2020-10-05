/*
 * anc_offline_identification.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_offline_identification.h"


#define ALGORITHM0      0u
#define ALGORITHM1      1u


void algorithmInit(
    anc_offline_identification_t*   self0,
    anc_offline_identification_t*   self1,
    dma_mem2mem_t*                  h_dma_mem2mem,
    uint32_t                        identificationCycles,
    uint32_t                        algorithmNum
);

/* Public methods definition */

void anc_offline_identification_init(
    anc_offline_identification_t*   self0,
    anc_offline_identification_t*   self1,
    dma_mem2mem_t*                  h_dma_mem2mem0,
    dma_mem2mem_t*                  h_dma_mem2mem1,
    uint32_t                        identificationCycles
)
{
    algorithmInit(self0, self1, h_dma_mem2mem0,
        identificationCycles, ALGORITHM0);
    algorithmInit(self1, self0, h_dma_mem2mem1,
        identificationCycles, ALGORITHM1);
}

/* Private methods definition */

void algorithmInit(
    anc_offline_identification_t*   self0,
    anc_offline_identification_t*   self1,
    dma_mem2mem_t*                  h_dma_mem2mem,
    uint32_t                        identificationCycles,
    uint32_t                        algorithmNum
)
{
    self0->h_dma_mem2mem        = h_dma_mem2mem;
    self0->identificationCycles = identificationCycles;
    self0->counter              = algorithmNum;
    self0->oldOut_p             = &(self1->out);

    self0->excitationSignal_p   = anc_lms_excitationSignal;

    /* Configure dma mem2mem transfer */
    dma_mem2mem_configure(h_dma_mem2mem,
        &(self0->states),
        &(self0->states) + 2 * sizeof(q15_t),
        (sizeof(self0->states) - 2 * sizeof(q15_t))
    );

    /* Init filter */
    fir_circular_init(
        &(self0->fir_Sn),
        anc_Sn_coeffs,
        fir_circular_getDataInPtr(&self1->fir_Sn),
        self0->states.Sn_state,
        ANC_SN_FILTER_LENGTH
    );

    /* Init LNLMS algorithm */
    lnlms_circular_init(
        &(self0->lnlms),
        anc_Wn_coeffs,
        ANC_OFFLINE_ALPHA,
        ANC_OFFLINE_MU,
        self0->states.Sn_state,
        ANC_SN_FILTER_LENGTH
    );
}
