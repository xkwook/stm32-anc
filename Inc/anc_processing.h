/*
 * anc_processing.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef ANC_PROCESSING_H_
#define ANC_PROCESSING_H_

#include "anc_math.h"

#include "anc_gain.h"
#include "agc.h"
#include "fir_circular_decimate.h"
#include "fir_circular_interp.h"
#include "iir3_circular.h"
#include "uart_transmitter.h"
#include "dma_mem2mem.h"

#define ANC_PROCESSING_CHUNK_SIZE    4
#define ANC_PROCESSING_OFFSET        2048

struct anc_processing_preprocessing_data_struct
{
    q15_t   refSample;
    q15_t   errSample;
};

typedef struct anc_processing_preprocessing_data_struct
    anc_processing_preprocessing_data_t;

struct anc_processing_struct
{
    agc_t*                  h_agc;
    uart_transmitter_t*     h_uart_transmitter;
    fir_circular_decimate_t fir_decimate_ref;
    fir_circular_decimate_t fir_decimate_err;
    iir3_circular_t         iir3_ref;
    iir3_circular_t         iir3_err;
    fir_circular_interp_t   fir_interp;
    q15_t                   refIn[ANC_PROCESSING_CHUNK_SIZE];
    q15_t                   errIn[ANC_PROCESSING_CHUNK_SIZE];
    q15_t                   out[ANC_PROCESSING_CHUNK_SIZE];
};

typedef struct anc_processing_struct anc_processing_t;

/* Public methods declaration */

void anc_processing_init(anc_processing_t* self);

inline anc_processing_preprocessing_data_t anc_processing_preprocessing(
    anc_processing_t*   self,
    uint16_t*           refMicBfr,
    uint16_t*           errMicBfr
);

inline void anc_processing_postprocessing(
    anc_processing_t*   self,
    q15_t               outSample,
    uint16_t*           outDacBfr
);

#endif /* ANC_PROCESSING_H_ */