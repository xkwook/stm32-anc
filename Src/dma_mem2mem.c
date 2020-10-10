/*
 * dma_mem2mem.c
 *
 *  Created on: Sep 16, 2020
 *      Author: klukomski
 */

#include "dma_mem2mem.h"
#include "stddef.h"

#define N_TEST		100

/* Private methods declaration */

static void test_finished_callback(
    dma_mem2mem_t*  self
);

static uint32_t test_pattern(uint32_t iteration);

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
    void    (*callback)(dma_mem2mem_t*)
)
{
    self->callback = callback;
}

int dma_mem2mem_configure(
    dma_mem2mem_t*  self,
    volatile void*  dest,
    volatile void*  source,
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

int dma_mem2mem_test(
    dma_mem2mem_t*  self
)
{
    static const int N      = N_TEST;
    static const int shift  = 20;
    static volatile uint32_t  memory[N_TEST];

    int retCode = DMA_MEM2MEM_SUCCESS;

    /* Init memory */
    for (uint32_t i = 0; i < N; i++)
    {
        memory[i] = test_pattern(i);
    }

    dma_mem2mem_setCallback(self,
        test_finished_callback);

    dma_mem2mem_configure(self,
        &memory[0],
        &memory[shift],
        sizeof(memory[0]) * (N - shift)
    );

    dma_mem2mem_start(self);

    /* Wait for finished callback */
    while (self->callback);

    /* Check memory after shifting */
    for (uint32_t i = 0; i < (N - shift); i++)
    {
        if (memory[i] != test_pattern(i + shift))
        {
            retCode = DMA_MEM2MEM_TEST_FAILED;
            break;
        }
    }

    return retCode;
}

static void test_finished_callback(
    dma_mem2mem_t*  self
)
{
    self->callback = NULL;
}

static uint32_t test_pattern(uint32_t iteration)
{
    return 0xDEAD0000 + iteration;
}
