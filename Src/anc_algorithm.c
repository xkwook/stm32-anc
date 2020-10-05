/*
 * anc_algorithm.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_algorithm.h"

/* Private methods declaration */

void algorithmInit(
    anc_algorithm_t*    self0,
    anc_algorithm_t*    self1,
    dma_mem2mem_t*      h_dma_mem2mem
);

/* Public methods definition */

void anc_algorithm_init(
    anc_algorithm_t*    self0,
    anc_algorithm_t*    self1,
    dma_mem2mem_t*      h_dma_mem2mem0,
    dma_mem2mem_t*      h_dma_mem2mem1
)
{
    algorithmInit(self0, self1, h_dma_mem2mem0);
    algorithmInit(self1, self0, h_dma_mem2mem1);
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

/* Private methods definition */

void algorithmInit(
    anc_algorithm_t*    self0,
    anc_algorithm_t*    self1,
    dma_mem2mem_t*      h_dma_mem2mem
)
{
    self0->h_dma_mem2mem = h_dma_mem2mem;
    self0->enable        = DISABLED;

    /* Configure dma mem2mem transfer */
    dma_mem2mem_configure(h_dma_mem2mem,
        &(self0->states),
        &(self0->states) + 2 * sizeof(q15_t),
        (sizeof(self0->states) - 2 * sizeof(q15_t))
    );

    /* Init filters */
    fir_circular_init(
        &(self0->fir_Sn),
        anc_Sn_coeffs,
        fir_circular_getDataInPtr(&self1->fir_Sn),
        self0->states.Sn_state,
        ANC_SN_FILTER_LENGTH
    );

    fir_circular_init(
        &(self0->fir_SnOut),
        NULL,
        fir_circular_getDataInPtr(&self1->fir_SnOut),
        self0->states.SnOut_state,
        ANC_WN_FILTER_LENGTH
    );

    fir_circular_init(
        &(self0->fir_Wn),
        anc_Wn_coeffs,
        fir_circular_getDataInPtr(&self1->fir_Wn),
        self0->states.Wn_state,
        ANC_WN_FILTER_LENGTH
    );

    /* Init LNLMS algorithm */
    lnlms_circular_init(
        &(self0->lnlms),
        anc_Wn_coeffs,
        ANC_ONLINE_ALPHA,
        ANC_ONLINE_MU,
        self0->states.SnOut_state,
        ANC_WN_FILTER_LENGTH
    );
}