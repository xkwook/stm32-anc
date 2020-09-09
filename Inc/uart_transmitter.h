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

#define UART_TRANSMITTER_BFR_MAXLEN     128
#define UART_TRANSMITTER_BFR_NUM        8

#define UART_TRANSMITTER_USART          USART1
#define UART_TRANSMITTER_DMA            DMA2
#define UART_TRANSMITTER_DMA_STREAM     LL_DMA_STREAM_7

#define UART_TRANSMITTER_SUCCESS        0x00


struct uart_transmitter_struct
{
    uint8_t nullByte;
};

typedef struct uart_transmitter_struct uart_transmitter_t;

/* Public methods declaration */

void uart_transmitter_init(uart_transmitter_t* self)
{
    /* Enable DMA mode in UART */
    LL_USART_EnableDMAReq_TX(UART_TRANSMITTER_USART);

    /* According to reference manual DMA stream configuration procedure */
    LL_DMA_SetPeriphAddress(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM,
        LL_USART_DMA_GetRegAddr(UART_TRANSMITTER_USART));

#if (UART_TRANSMITTER_DMA_STREAM == LL_DMA_STREAM_7)
    LL_DMA_ClearFlag_TC7(UART_TRANSMITTER_DMA);
#endif

    /* Enable Transfer Completed interrupt */
    LL_DMA_EnableIT_TC(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM);
}

int uart_transmitter_setMsg(uart_transmitter_t* self, uint8_t* msg, uint32_t length)
{
    if (LL_DMA_IsEnabledStream(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM))
    {
        /* Return error */
        return -1;  // need to do error
    }

    LL_DMA_SetMemoryAddress(
        UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM,
        (uint32_t)msg
    );
    LL_DMA_SetDataLength(
        UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM,
        length
    );

    return UART_TRANSMITTER_SUCCESS;
}

int uart_transmitter_send(uart_transmitter_t* self)
{
    if (LL_DMA_IsEnabledStream(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM))
    {
        /* Return error */
        return -1;  // need to do error
    }

    LL_DMA_EnableStream(UART_TRANSMITTER_DMA, UART_TRANSMITTER_DMA_STREAM);

    /* Return success */
    return UART_TRANSMITTER_SUCCESS;
}

void uart_transmitter_dmaIrqHandler(uart_transmitter_t* self)
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
