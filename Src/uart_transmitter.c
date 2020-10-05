/*
 * uart_transmitter.c
 *
 *  Created on: Sep 12, 2020
 *      Author: klukomski
 */

#include "uart_transmitter.h"


/* Public methods definition */

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
        return UART_TRANSMITTER_BUSY;
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
