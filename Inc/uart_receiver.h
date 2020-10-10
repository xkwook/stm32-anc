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


struct uart_receiver_struct
{
    volatile char* volatile  read_p;
    volatile char* volatile  write_p;
    volatile char*           last_p;
    char  volatile  bfr[UART_RECEIVER_BFR_NUM][UART_RECEIVER_BFR_MAXLEN];
};

typedef struct uart_receiver_struct uart_receiver_t;

/* Public methods declaration */

void uart_receiver_init(uart_receiver_t* self);

int uart_receiver_start(uart_receiver_t* self);

void uart_receiver_stop(uart_receiver_t* self);

char* uart_receiver_getMsg(uart_receiver_t* self);

void uart_receiver_freeMsg(uart_receiver_t* self);

__attribute__((weak)) void uart_receiver_onQueueFullCallback(uart_receiver_t* self);

void uart_receiver_dmaIrqHandler(uart_receiver_t* self);

void uart_receiver_uartIrqHandler(uart_receiver_t* self);

#endif /* UART_RECEIVER_H_ */
