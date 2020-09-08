/*
 * anc_acquisition.c
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#include "anc_acquisition.h"

#include <stddef.h>

/* Private methods declaration */

static void initDma(uint16_t* bfr, uint32_t regAddr, DMA_TypeDef *DMAx, uint32_t Stream);
static void initAdcDma(uint16_t* bfr, ADC_TypeDef *ADCx, DMA_TypeDef *DMAx, uint32_t Stream);
static void initDacDma(uint16_t* bfr, DAC_TypeDef *DACx, uint32_t DAC_Channel, DMA_TypeDef *DMAx, uint32_t Stream);
static void enableTransfersInterrupts(DMA_TypeDef *DMAx, uint32_t Stream);
static void disableTransfersInterrupts(DMA_TypeDef *DMAx, uint32_t Stream);

static inline uint32_t IsActiveMasterFlag_HT();
static inline uint32_t IsActiveMasterFlag_TC();
static inline uint32_t IsActiveSlavesFlag_HT();
static inline uint32_t IsActiveSlavesFlag_TC();
static inline void ClearFlags_HT();
static inline void ClearFlags_TC();

/* Public methods definition */

void anc_acquisition_init(anc_acquisition_t* self)
{
    /* Init DAC values with constant offset at "zero" */
    for (uint32_t i = 0; i < ANC_ACQUISITION_BFR_LENGTH; i++)
    {
        self->outDacBfr[i] = ANC_DAC_OFFSET;
    }

    /* Init DMA circular transfers */
    initAdcDma(self->refMicBfr, ANC_REF_MIC_ADC,
        ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM);
    initAdcDma(self->errMicBfr, ANC_ERR_MIC_ADC,
        ANC_ERR_MIC_DMA, ANC_ERR_MIC_DMA_STREAM);
    initDacDma(self->outDacBfr, ANC_OUT_DAC,
        ANC_OUT_DAC_CHANNEL, ANC_OUT_DAC_DMA,
        ANC_OUT_DAC_DMA_STREAM);
}

int  anc_acquisition_configure
(
    anc_acquisition_t*              self,
    uint32_t                        chunkSize,
    anc_acquisition_bfr_callback_t  halfBfrCallback,
    anc_acquisition_bfr_callback_t  fullBfrCallback
)
{
    if (    LL_DMA_IsEnabledStream(ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM)
        ||  LL_DMA_IsEnabledStream(ANC_ERR_MIC_DMA, ANC_ERR_MIC_DMA_STREAM)
        ||  LL_DMA_IsEnabledStream(ANC_OUT_DAC_DMA, ANC_OUT_DAC_DMA_STREAM))
    {
        /* Return error */
        return -1;  // need to do error
    }
    if (chunkSize > ANC_ACQUISITION_CHUNK_MAXSIZE)
    {
        return -2;
    }
    if ((halfBfrCallback == NULL) || (fullBfrCallback == NULL))
    {
        return -3;
    }

    self->chunkSize         = chunkSize;
    self->halfBfrCallback   = halfBfrCallback;
    self->fullBfrCallback   = fullBfrCallback;
    LL_DMA_SetDataLength(ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM, self->chunkSize * 2);
    LL_DMA_SetDataLength(ANC_ERR_MIC_DMA, ANC_ERR_MIC_DMA_STREAM, self->chunkSize * 2);
    LL_DMA_SetDataLength(ANC_OUT_DAC_DMA, ANC_OUT_DAC_DMA_STREAM, self->chunkSize * 2);

    return 0;
}

void anc_acquisition_start(anc_acquisition_t* self)
{
    /* Enable Master Transfers interrupt */
    enableTransfersInterrupts(
        ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM);

    LL_DMA_EnableStream(ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM);
    LL_DMA_EnableStream(ANC_ERR_MIC_DMA, ANC_ERR_MIC_DMA_STREAM);
    LL_DMA_EnableStream(ANC_OUT_DAC_DMA, ANC_OUT_DAC_DMA_STREAM);

    LL_TIM_EnableCounter(ANC_TRIGGER_TIMER);
}

void anc_acquisition_stop(anc_acquisition_t* self)
{
    LL_TIM_DisableCounter(ANC_TRIGGER_TIMER);

    disableTransfersInterrupts(
        ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM);

    LL_DMA_DisableStream(ANC_REF_MIC_DMA, ANC_REF_MIC_DMA_STREAM);
    LL_DMA_DisableStream(ANC_ERR_MIC_DMA, ANC_ERR_MIC_DMA_STREAM);
    LL_DMA_DisableStream(ANC_OUT_DAC_DMA, ANC_OUT_DAC_DMA_STREAM);
}

void anc_acquisition_dmaIrqHandler(anc_acquisition_t* self)
{
    /* Half Transfer interrupt */
    if (IsActiveMasterFlag_HT())
    {
        /* Wait for all Slaves DMA transfers to complete */
        while(!IsActiveSlavesFlag_HT());
        ClearFlags_HT();
        self->halfBfrCallback(
            self->refMicBfr,
            self->errMicBfr,
            self->outDacBfr
        );
    }
    /* Transfer Completed interrupt */
    if (IsActiveMasterFlag_TC())
    {
        /* Wait for all Slaves DMA transfers to complete */
        while(!IsActiveSlavesFlag_TC());
        ClearFlags_TC();
        self->fullBfrCallback(
            &self->refMicBfr[self->chunkSize],
            &self->errMicBfr[self->chunkSize],
            &self->outDacBfr[self->chunkSize]
        );
    }
}

/* Private methods definition */

static void initDma(uint16_t* bfr, uint32_t regAddr, DMA_TypeDef *DMAx, uint32_t Stream)
{
    /* According to reference manual DMA stream configuration procedure */
    LL_DMA_SetPeriphAddress(DMAx, Stream,
        regAddr);
    LL_DMA_SetMemoryAddress(DMAx, Stream,
        (uint32_t)bfr);
}

static void initAdcDma(uint16_t* bfr, ADC_TypeDef *ADCx, DMA_TypeDef *DMAx, uint32_t Stream)
{
    uint32_t regAddr = LL_ADC_DMA_GetRegAddr(ADCx, LL_ADC_DMA_REG_REGULAR_DATA);

    /* Maybe there should be enabled DDS bit that isn't enabled for circular mode by CUBE MX */
    LL_ADC_REG_SetDMATransfer(ADCx, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

    /* Enable ADC */
    LL_ADC_Enable(ADCx);

    initDma(bfr, regAddr, DMAx, Stream);
}

static void initDacDma(uint16_t* bfr, DAC_TypeDef *DACx, uint32_t DAC_Channel, DMA_TypeDef *DMAx, uint32_t Stream)
{
    uint32_t regAddr = LL_DAC_DMA_GetRegAddr(DACx, DAC_Channel,
        LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED);

    /* Enable DMA requests in DAC */
    LL_DAC_EnableDMAReq(DACx, DAC_Channel);

    /* Enable DAC Trigger */
    LL_DAC_EnableTrigger(DACx, DAC_Channel);

    /* Enable DAC */
    LL_DAC_Enable(DACx, DAC_Channel);

    initDma(bfr, regAddr, DMAx, Stream);
}

static void enableTransfersInterrupts(DMA_TypeDef *DMAx, uint32_t Stream)
{
    /* Clear all flags */
    ClearFlags_HT();
    ClearFlags_TC();

    /* Enable Half Transfer interrupt */
    LL_DMA_EnableIT_HT(DMAx, Stream);

    /* Enable Transfer Completed interrupt */
    LL_DMA_EnableIT_TC(DMAx, Stream);
}

static void disableTransfersInterrupts(DMA_TypeDef *DMAx, uint32_t Stream)
{
    /* Disable Half Transfer interrupt */
    LL_DMA_DisableIT_HT(DMAx, Stream);

    /* Disable Transfer Completed interrupt */
    LL_DMA_DisableIT_TC(DMAx, Stream);

    /* Clear all flags */
    ClearFlags_HT();
    ClearFlags_TC();
}

static inline uint32_t IsActiveMasterFlag_HT()
{
    return LL_DMA_IsActiveFlag_HT0(ANC_REF_MIC_DMA);
}

static inline uint32_t IsActiveMasterFlag_TC()
{
    return LL_DMA_IsActiveFlag_TC0(ANC_REF_MIC_DMA);
}

static inline uint32_t IsActiveSlavesFlag_HT()
{
    return (LL_DMA_IsActiveFlag_HT2(ANC_ERR_MIC_DMA)
        && LL_DMA_IsActiveFlag_HT6(ANC_OUT_DAC_DMA));
}

static inline uint32_t IsActiveSlavesFlag_TC()
{
    return (LL_DMA_IsActiveFlag_TC2(ANC_ERR_MIC_DMA)
        && LL_DMA_IsActiveFlag_TC6(ANC_OUT_DAC_DMA));
}

static inline void ClearFlags_HT()
{
    LL_DMA_ClearFlag_HT0(ANC_REF_MIC_DMA);
    LL_DMA_ClearFlag_HT2(ANC_ERR_MIC_DMA);
    LL_DMA_ClearFlag_HT6(ANC_OUT_DAC_DMA);
}

static inline void ClearFlags_TC()
{
    LL_DMA_ClearFlag_TC0(ANC_REF_MIC_DMA);
    LL_DMA_ClearFlag_TC2(ANC_ERR_MIC_DMA);
    LL_DMA_ClearFlag_TC6(ANC_OUT_DAC_DMA);
}
