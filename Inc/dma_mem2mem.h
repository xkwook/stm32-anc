/*
 * dma_mem2mem.h
 *
 *  Created on: Sep 16, 2020
 *      Author: klukomski
 */

#ifndef DMA_MEM2MEM_H_
#define DMA_MEM2MEM_H_

#include "main.h"

#define DMA_MEM2MEM_SUCCESS     0x00
#define DMA_MEM2MEM_BUSY        -0x01

#define DMA_MEM2MEM_READY       0x00

struct dma_mem2mem_struct
{
    uint32_t        ready;
    DMA_TypeDef*    DMAx;
    uint32_t        Stream;
    void    (*callback)(struct dma_mem2mem_struct*);
};

typedef struct dma_mem2mem_struct dma_mem2mem_t;

/* Private inline methods declaration */

static inline uint32_t dma_mem2mem_IsActiveFlag_TC(dma_mem2mem_t* self);

static inline void dma_mem2mem_ClearFlag_TC(dma_mem2mem_t* self);

/* Public methods declaration */

void dma_mem2mem_init(
    dma_mem2mem_t*  self,
    DMA_TypeDef*    DMAx,
    uint32_t        Stream,
    void    (*callback)(struct dma_mem2mem_struct*)
);

int dma_mem2mem_configure(
    dma_mem2mem_t*  self,
    void*           dest,
    void*           source,
    uint32_t        length
);

/* Public inline methods */

static inline int dma_mem2mem_isBusy(dma_mem2mem_t* self)
{
    return LL_DMA_IsEnabledStream(self->DMAx, self->Stream);
}

static inline void dma_mem2mem_start(dma_mem2mem_t* self)
{
    LL_DMA_EnableStream(self->DMAx, self->Stream);
}

static inline void dma_mem2mem_dmaIrqHandler(dma_mem2mem_t* self)
{
    if (dma_mem2mem_IsActiveFlag_TC(self))
    {
        dma_mem2mem_ClearFlag_TC(self);
        if (self->callback)
        {
            self->callback(self);
        }
    }
}

/* Private inline methods definition */

static inline uint32_t dma_mem2mem_IsActiveFlag_TC(dma_mem2mem_t* self)
{
    uint32_t retVal;
    switch (self->stream)
    {
    case LL_DMA_STREAM_0:
        retVal = LL_DMA_IsActiveFlag_TC0(self->DMAx);
        break;
    case LL_DMA_STREAM_1:
        retVal = LL_DMA_IsActiveFlag_TC1(self->DMAx);
        break;
    case LL_DMA_STREAM_2:
        retVal = LL_DMA_IsActiveFlag_TC2(self->DMAx);
        break;
    case LL_DMA_STREAM_3:
        retVal = LL_DMA_IsActiveFlag_TC3(self->DMAx);
        break;
    case LL_DMA_STREAM_4:
        retVal = LL_DMA_IsActiveFlag_TC4(self->DMAx);
        break;
    case LL_DMA_STREAM_5:
        retVal = LL_DMA_IsActiveFlag_TC5(self->DMAx);
        break;
    case LL_DMA_STREAM_6:
        retVal = LL_DMA_IsActiveFlag_TC6(self->DMAx);
        break;
    case LL_DMA_STREAM_7:
        retVal = LL_DMA_IsActiveFlag_TC7(self->DMAx);
        break;
    }

    return retVal;
}

static inline void dma_mem2mem_ClearFlag_TC(dma_mem2mem_t* self)
{
    switch (self->stream)
    {
    case LL_DMA_STREAM_0:
        LL_DMA_ClearFlag_TC0(self->DMAx);
        break;
    case LL_DMA_STREAM_1:
        LL_DMA_ClearFlag_TC1(self->DMAx);
        break;
    case LL_DMA_STREAM_2:
        LL_DMA_ClearFlag_TC2(self->DMAx);
        break;
    case LL_DMA_STREAM_3:
        LL_DMA_ClearFlag_TC3(self->DMAx);
        break;
    case LL_DMA_STREAM_4:
        LL_DMA_ClearFlag_TC4(self->DMAx);
        break;
    case LL_DMA_STREAM_5:
        LL_DMA_ClearFlag_TC5(self->DMAx);
        break;
    case LL_DMA_STREAM_6:
        LL_DMA_ClearFlag_TC6(self->DMAx);
        break;
    case LL_DMA_STREAM_7:
        LL_DMA_ClearFlag_TC7(self->DMAx);
        break;
    }
}

#endif /* DMA_MEM2MEM_H_ */