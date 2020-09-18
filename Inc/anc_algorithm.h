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


typedef struct __attribute__((packed, aligned(sizeof(uint32_t)))))
{

} anc_algorithm_data_t;

struct anc_algorithm_struct
{
    dma_mem2mem_t*          h_dma_mem2mem;
    fir_circular_t          fir_Sn;
    fir_circular_t          fir_Wn;
    lnlms_circular_t        lnlms;
    uint32_t                enable;
    anc_algorithm_data_t    data;
};

typedef struct anc_algorithm_struct anc_algorithm_t;


/* Public methods declaration */

void anc_algorithm_init(anc_algorithm_t* self);

void anc_algorithm_enable(anc_algorithm_t* self);

void anc_algorithm_disable(anc_algorithm_t* self);

inline q15_t anc_algorithm_calculate(
    anc_algorithm_t*                    self,
    anc_processing_preprocessing_data_t samples
);

#endif /* ANC_ALGORITHM_H_ */