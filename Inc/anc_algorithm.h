/*
 * anc_algorithm.h
 *
 *  Created on: Sep 16, 2020
 *      Author: klukomski
 */

#ifndef ANC_ALGORITHM_H_
#define ANC_ALGORITHM_H_

#include <stdint.h>

#include "dma_mem2mem.h"

typedef struct __attribute__((packed, aligned(sizeof(uint32_t)))))
{

} anc_algorithm_data_t;

struct anc_algorithm_struct
{
    anc_algorithm_data_t    data;
};

typedef struct anc_algorithm_struct anc_algorithm_t;

void anc_algorithm_init(anc_algorithm_t* self);

void anc_algorithm_enable(anc_algorithm_t* self);

void anc_algorithm_disable(anc_algorithm_t* self);

void anc_algorithm_calculate(anc_algorithm_t* self)
{
    /* Set new gains ? */

    /* At first AGC ? */

    /* Get data -> IIR */

    /* Then FIR and decimate */

    /* Filter Ref with Sn Path */

    /* Update Filtered-X squared sum */

    /* Update Wn using LNFxLMS algorithm */

    /* Filter Ref with Wn Path and calculate u */

    /* Interpolate u with FIR and set to DAC */

    /* Run DMA mem2mem for shifting states */
}

#endif /* ANC_ALGORITHM_H_ */