/*
 * uart_transmitter.h
 *
 *  Created on: Sep 9, 2020
 *      Author: klukomski
 */

#ifndef UART_TRANSMITTER_H_
#define UART_TRANSMITTER_H_

#include <stddef.h>
#include "main.h"

#define UART_TRANSMITTER_USART          USART1
#define UART_TRANSMITTER_DMA            DMA2
#define UART_TRANSMITTER_DMA_STREAM     LL_DMA_STREAM_7

#define UART_TRANSMITTER_SUCCESS        0x00
#define UART_TRANSMITTER_READY          0x00
#define UART_TRANSMITTER_BUSY           -0x01

struct uart_transmitter_struct
{
    uint8_t nullByte;
};

typedef struct uart_transmitter_struct uart_transmitter_t;

/* Public methods declaration */

void uart_transmitter_init(uart_transmitter_t* self);

int uart_transmitter_setMsg(uart_transmitter_t* self, uint8_t* msg, uint32_t length);

__attribute__((weak)) void uart_transmitter_transferCpltCallback(uart_transmitter_t* self);

/* Inline methods */

static inline int uart_transmitter_isBusy(uart_transmitter_t* self)
{
    return LL_DMA_IsEnabledStream(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM);
}

static inline void uart_transmitter_start(uart_transmitter_t* self)
{
    LL_DMA_EnableStream(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM);
}

static inline void uart_transmitter_dmaIrqHandler(uart_transmitter_t* self)
{
    uint32_t transferCompletedFlag;
#if (UART_TRANSMITTER_DMA_STREAM == LL_DMA_STREAM_7)
    transferCompletedFlag = LL_DMA_IsActiveFlag_TC7(UART_TRANSMITTER_DMA);
#endif
    if (transferCompletedFlag)
    {
#if (UART_TRANSMITTER_DMA_STREAM == LL_DMA_STREAM_7)
        LL_DMA_ClearFlag_TC7(UART_TRANSMITTER_DMA);
#endif
        uart_transmitter_transferCpltCallback(self);
    }
}

#endif /* UART_TRANSMITTER_H_ */
