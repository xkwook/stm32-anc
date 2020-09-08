/*
 * anc_acquisition.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_ACQUISITION_H_
#define ANC_ACQUISITION_H_

#include "main.h"

#define ANC_TRIGGER_TIMER           TIM2

#define ANC_REF_MIC_ADC             ADC1
#define ANC_REF_MIC_DMA             DMA2
#define ANC_REF_MIC_DMA_STREAM      LL_DMA_STREAM_0

#define ANC_ERR_MIC_ADC             ADC2
#define ANC_ERR_MIC_DMA             DMA2
#define ANC_ERR_MIC_DMA_STREAM      LL_DMA_STREAM_2

#define ANC_OUT_DAC                 DAC
#define ANC_OUT_DAC_CHANNEL         LL_DAC_CHANNEL_2
#define ANC_OUT_DAC_DMA             DMA1
#define ANC_OUT_DAC_DMA_STREAM      LL_DMA_STREAM_6

#define ANC_ACQUISITION_CHUNK_SIZE  4
#define ANC_ACQUISITION_BFR_LENGTH  (ANC_ACQUISITION_CHUNK_SIZE * 2)

#define ANC_DAC_OFFSET              2048

struct anc_acquisition_struct {
    uint16_t    refMicBfr[ANC_ACQUISITION_BFR_LENGTH];
    uint16_t    errMicBfr[ANC_ACQUISITION_BFR_LENGTH];
    uint16_t    outDacBfr[ANC_ACQUISITION_BFR_LENGTH];
};

typedef struct anc_acquisition_struct anc_acquisition_t;

/* Public methods declaration */

void anc_acquisition_init(anc_acquisition_t* self);

void anc_acquisition_start(anc_acquisition_t* self);

void anc_acquisition_stop(anc_acquisition_t* self);

void anc_acquisition_dmaIrqHandler(anc_acquisition_t* self);

__attribute__((__weak__)) void anc_acquisition_bfr0_callback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr);

__attribute__((__weak__)) void anc_acquisition_bfr1_callback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr);

#endif /* ANC_ACQUISITION_H_ */
