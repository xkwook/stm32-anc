/*
 * dma_mem2mem.c
 *
 *  Created on: Sep 16, 2020
 *      Author: klukomski
 */

#include "dma_mem2mem.h"
#include "stddef.h"

/* Private methods declaration */

static inline uint32_t IsActiveFlag_TC(dma_mem2mem_t* self);
static inline void ClearFlag_TC(dma_mem2mem_t* self);

/* Public methods definition */

void dma_mem2mem_init(
    dma_mem2mem_t*  self,
    DMA_TypeDef*    DMAx,
    uint32_t        Stream
)
{
    self->DMAx      = DMAx;
    self->Stream    = Stream;
    self->callback  = NULL;

    /* Clear flag to make sure no interrupt will be generated */
    ClearFlag_TC(self);

    /* Enable Transfer Completed interrupt */
    LL_DMA_EnableIT_TC(self->DMAx, self->Stream);
}

void dma_mem2mem_setCallback(
    dma_mem2mem_t*  self,
    void    (*callback)(struct dma_mem2mem_struct*)
)
{
    self->callback = callback;
}

int dma_mem2mem_configure(
    dma_mem2mem_t*  self,
    void*           dest,
    void*           source,
    uint32_t        length
)
{
    if (LL_DMA_IsEnabledStream(self->DMAx, self->Stream))
    {
        /* Return error */
        return DMA_MEM2MEM_BUSY;
    }

    /* Configure transfer in registers */

    LL_DMA_SetM2MSrcAddress(self->DMAx, self->Stream
        (uint32_t)source
    );

    LL_DMA_SetM2MDstAddress(self->DMAx, self->Stream,
        (uint32_t)dest
    );

    LL_DMA_SetDataLength(self->DMAx, self->Stream,
        length
    );

    return DMA_MEM2MEM_SUCCESS;
}

int dma_mem2mem_isReady(dma_mem2mem_t* self)
{

}

int dma_mem2mem_start(dma_mem2mem_t* self)
{
    if (LL_DMA_IsEnabledStream(self->DMAx, self->Stream))
    {
        /* Return error */
        return DMA_MEM2MEM_BUSY;
    }

    LL_DMA_EnableStream(self->DMAx, self->Stream);

    return DMA_MEM2MEM_SUCCESS;
}

void dma_mem2mem_dmaIrqHandler(dma_mem2mem_t* self)
{
    if (IsActiveFlag_TC(self))
    {
        ClearFlag_TC(self);
        if (self->callback)
        {
            self->callback(self);
        }
    }
}

/* Private methods definition */

static inline uint32_t IsActiveFlag_TC(dma_mem2mem_t* self)
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

static inline void ClearFlag_TC(dma_mem2mem_t* self)
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