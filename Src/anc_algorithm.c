/*
 * anc_algorithm.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_algorithm.h"


#define DISABLED        0
#define ENABLED         1


/* Public methods definition */

void anc_algorithm_init(anc_algorithm_t* self)
{

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

inline q15_t anc_algorithm_calculate(
    anc_algorithm_t*                    self,
    anc_processing_preprocessing_data_t samples
)
{
    q15_t refSn;
    q15_t result;

    if (self->enable == ENABLED)
    {
        /* TODO: Has DMA mem2mem finished? */

        /* Filter Ref with Sn Path */
        fir_circular_pushData(&(self->fir_Sn), samples.refSample);
        refSn = fir_circular_calculate(&(self->fir_Sn));

        /* TODO: Append refSn as filter state (maybe use fir circular) */

        /* Update Wn using LNLMS algorithm */
        lnlms_circular_update(self->lnlms, samples.errSample);

        /* Filter Ref with Wn Path and calculate u */
        fir_circular_pushData(&(self->fir_Wn), samples.refSample);
        result = fir_circular_calculate(&(self->fir_Sn));

        /* TODO: Run DMA mem2mem for shifting states */
    }
    else
    {
        /* No output waveform generated */
        result = 0;
    }

    return result;
}