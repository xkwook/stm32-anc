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

#define DMA_MEM2MEM_READY       0x01

struct dma_mem2mem_struct
{
    uint32_t        ready;
    DMA_TypeDef*    DMAx;
    uint32_t        Stream;
    void    (*callback)(struct dma_mem2mem_struct*);
};

typedef struct dma_mem2mem_struct dma_mem2mem_t;

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

int dma_mem2mem_isReady(dma_mem2mem_t* self);

int dma_mem2mem_start(dma_mem2mem_t* self);

void dma_mem2mem_dmaIrqHandler(dma_mem2mem_t* self);

#endif /* DMA_MEM2MEM_H_ */