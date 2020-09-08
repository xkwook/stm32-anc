/*
 * uart_receiver.c
 *
 *  Created on: Sep 6, 2020
 *      Author: klukomski
 */

#include "uart_receiver.h"

#define DMA_TRANSFER_LENGTH     (UART_RECEIVER_BFR_MAXLEN - 1)

#define CRITICAL_SECTION_BEGIN()  {                                     \
    /* Disable Transfer Completed interrupt */                          \
    LL_DMA_DisableIT_TC(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);   \
    }

#define CRITICAL_SECTION_END()  {                                       \
    /* Enable Transfer Completed interrupt */                           \
    LL_DMA_EnableIT_TC(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);    \
    }


/* Private methods declaration */

static char* next_buffer_ptr(uart_receiver_t* self, char* ptr);

/* Public methods definition */

void uart_receiver_init(uart_receiver_t* self)
{
    self->read_p    = NULL;
    self->write_p   = &(self->bfr[0][0]);
    self->last_p    = &(self->bfr[UART_RECEIVER_BFR_NUM - 1][0]);

    /* Terminate all strings at the end */
    for (int i = 0; i < UART_RECEIVER_BFR_NUM; i++)
    {
        self->bfr[i][UART_RECEIVER_BFR_MAXLEN - 1] = '\0';
    }

    /* Enable DMA mode in UART */
    LL_USART_EnableDMAReq_RX(UART_RECEIVER_USART);

    /* According to reference manual DMA stream configuration procedure */
    LL_DMA_SetPeriphAddress(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM,
        LL_USART_DMA_GetRegAddr(UART_RECEIVER_USART));
    LL_DMA_SetMemoryAddress(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM,
        (uint32_t)self->write_p);
    LL_DMA_SetDataLength(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM,
        DMA_TRANSFER_LENGTH);
}

int uart_receiver_start(uart_receiver_t* self)
{
    if (LL_DMA_IsEnabledStream(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM))
    {
        /* Return already started error */
        return UART_RECEIVER_ALREADY_STARTED;
    }

    /* Clear interrupt flags */
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
    LL_DMA_ClearFlag_TC5(UART_RECEIVER_DMA);
#endif
    LL_USART_ClearFlag_IDLE(UART_RECEIVER_USART);

    /* Enable Idle line interrupt */
    LL_USART_EnableIT_IDLE(UART_RECEIVER_USART);

    /* Enable Transfer Completed interrupt */
    LL_DMA_EnableIT_TC(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);

    /* Start DMA stream */
    LL_DMA_EnableStream(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);

    return UART_RECEIVER_SUCCESS;
}

void uart_receiver_stop(uart_receiver_t* self)
{
    /* Disable Idle line interrupt */
    LL_USART_DisableIT_IDLE(UART_RECEIVER_USART);

    /* Disable Transfer Completed interrupt */
    LL_DMA_DisableIT_TC(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);

    /* Stop DMA stream */
    LL_DMA_DisableStream(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);

    /* Clear interrupt flags */
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
    LL_DMA_ClearFlag_TC5(UART_RECEIVER_DMA);
#endif
    LL_USART_ClearFlag_IDLE(UART_RECEIVER_USART);
}

char* uart_receiver_getMsg(uart_receiver_t* self)
{
    return self->read_p;
}

void uart_receiver_freeMsg(uart_receiver_t* self)
{
    char* nextBfr_p;
    nextBfr_p = next_buffer_ptr(self, self->read_p);
    CRITICAL_SECTION_BEGIN();
    if (nextBfr_p == self->write_p)
    {
        self->read_p = NULL;
    }
    else
    {
        self->read_p = nextBfr_p;
    }
    CRITICAL_SECTION_END();
}

void uart_receiver_dmaIrqHandler(uart_receiver_t* self)
{
    uint32_t transferCompletedFlag;
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
    transferCompletedFlag = LL_DMA_IsActiveFlag_TC5(UART_RECEIVER_DMA);
#endif
    if (transferCompletedFlag)
    {
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
        LL_DMA_ClearFlag_TC5(UART_RECEIVER_DMA);
#endif
        /* If buffer was empty, set read pointer as first element */
        if (self->read_p == NULL)
        {
            self->read_p = self->write_p;
        }
        self->write_p = next_buffer_ptr(self, self->write_p);
        /* If read and write are the same, there is QueueFull error.
         * No more transmissions will occur.
         */
        if (self->write_p == self->read_p)
        {
            uart_receiver_onQueueFullCallback(self);
        }
        else
        {
            /* Update memory address and start DMA stream */
            LL_DMA_SetMemoryAddress(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM,
                (uint32_t)self->write_p);
            LL_DMA_EnableStream(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);
        }
    }
}

void uart_receiver_uartIrqHandler(uart_receiver_t* self)
{
    if (LL_USART_IsActiveFlag_IDLE(UART_RECEIVER_USART))
    {
        LL_USART_ClearFlag_IDLE(UART_RECEIVER_USART);
        uint32_t remainingBytes = LL_DMA_GetDataLength(
            UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);
        /* Terminate string received */
        self->write_p[DMA_TRANSFER_LENGTH - remainingBytes] = '\0';
        /* Terminate DMA string transaction and generete TC interrupt */
        LL_DMA_DisableStream(UART_RECEIVER_DMA,
            UART_RECEIVER_DMA_STREAM);
    }
}

/* Private methods definition */

static char* next_buffer_ptr(uart_receiver_t* self, char* ptr)
{
    char* ret_p;

    if (ptr == self->last_p)
    {
        /* Move to first buffer */
        ret_p = &(self->bfr[0][0]);
    }
    else
    {
        /* Move to next buffer */
        ret_p = ptr + UART_RECEIVER_BFR_MAXLEN;
    }

    return ret_p;
}
