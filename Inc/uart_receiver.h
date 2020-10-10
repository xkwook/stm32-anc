/*
 * uart_receiver.h
 *
 *  Created on: Sep 6, 2020
 *      Author: klukomski
 */

#ifndef UART_RECEIVER_H_
#define UART_RECEIVER_H_

#include <stddef.h>
#include "main.h"

#define UART_RECEIVER_BFR_MAXLEN        128
#define UART_RECEIVER_BFR_NUM           8

#define UART_RECEIVER_USART             USART1
#define UART_RECEIVER_DMA               DMA2
#define UART_RECEIVER_DMA_STREAM        LL_DMA_STREAM_5

#define UART_RECEIVER_SUCCESS           0x00
#define UART_RECEIVER_ALREADY_STARTED   -0x01
#define UART_RECEIVER_NO_MSG            NULL

#define UART_RECEIVER_DMA_TRANSFER_LENGTH   \
    (UART_RECEIVER_BFR_MAXLEN - 1)


struct uart_receiver_struct
{
    volatile char*  read_p;
    volatile char*  write_p;
    volatile char*  last_p;
    char            bfr[UART_RECEIVER_BFR_NUM][UART_RECEIVER_BFR_MAXLEN];
};

typedef volatile struct uart_receiver_struct uart_receiver_t;

/* Private methods declaration */

static inline volatile char* uart_receiver_next_buffer_ptr(
    uart_receiver_t*    self,
    volatile char*      ptr
);

/* Public methods declaration */

void uart_receiver_init(uart_receiver_t* self);

int uart_receiver_start(uart_receiver_t* self);

void uart_receiver_stop(uart_receiver_t* self);

volatile char* uart_receiver_getMsg(uart_receiver_t* self);

void uart_receiver_freeMsg(uart_receiver_t* self);

__attribute__((weak)) void uart_receiver_onQueueFullCallback(uart_receiver_t* self);

/* Inline methods */

static inline void uart_receiver_dmaIrqHandler(uart_receiver_t* self)
{
    uint32_t transferCompletedFlag;
    volatile char* read_p;
    volatile char* write_p;
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
    transferCompletedFlag = LL_DMA_IsActiveFlag_TC5(UART_RECEIVER_DMA);
#endif
    if (transferCompletedFlag)
    {
#if (UART_RECEIVER_DMA_STREAM == LL_DMA_STREAM_5)
        LL_DMA_ClearFlag_TC5(UART_RECEIVER_DMA);
#endif
        /* Load read and write pointers */
        read_p  = self->read_p;
        write_p = self->write_p;

        /* If buffer was empty, set read pointer as first element */
        if (read_p == NULL)
        {
            read_p = write_p;
        }
        write_p = uart_receiver_next_buffer_ptr(self, write_p);
        /* If read and write are the same, there is QueueFull error.
         * No more transmissions will occur.
         */
        if (write_p == read_p)
        {
            uart_receiver_onQueueFullCallback(self);
        }
        else
        {
            /* Update memory address and start DMA stream */
            LL_DMA_SetMemoryAddress(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM,
                (uint32_t) write_p);
            LL_DMA_EnableStream(UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);
        }

        /* Store read and write pointers */
        self->read_p  = read_p;
        self->write_p = write_p;
    }
}

static inline void uart_receiver_uartIrqHandler(uart_receiver_t* self)
{
    if (LL_USART_IsActiveFlag_IDLE(UART_RECEIVER_USART))
    {
        LL_USART_ClearFlag_IDLE(UART_RECEIVER_USART);
        uint32_t remainingBytes = LL_DMA_GetDataLength(
            UART_RECEIVER_DMA, UART_RECEIVER_DMA_STREAM);
        /* Terminate string received */
        self->write_p[UART_RECEIVER_DMA_TRANSFER_LENGTH - remainingBytes] = '\0';
        /* Terminate DMA string transaction and generete TC interrupt */
        LL_DMA_DisableStream(UART_RECEIVER_DMA,
            UART_RECEIVER_DMA_STREAM);
    }
}

/* Private methods definition */

static inline volatile char* uart_receiver_next_buffer_ptr(
    uart_receiver_t*    self,
    volatile char*      ptr
)
{
    volatile char* ret_p;

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

#endif /* UART_RECEIVER_H_ */
