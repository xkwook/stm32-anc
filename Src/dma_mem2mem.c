/*
 * dma_mem2mem.c
 *
 *  Created on: Sep 16, 2020
 *      Author: klukomski
 */

#include "dma_mem2mem.h"
#include "stddef.h"

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
    dma_mem2mem_ClearFlag_TC(self);

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

    LL_DMA_SetM2MSrcAddress(self->DMAx, self->Stream,
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
