/*
 * anc_application.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_APPLICATION_H_
#define ANC_APPLICATION_H_

#include "anc_acquisition.h"
#include "uart_receiver.h"
#include "uart_transmitter.h"
#include "dma_mem2mem.h"

typedef enum
{
    ANC_APPLICATION_IDLE,
    ANC_APPLICATION_ACQUISITION,
    ANC_APPLICATION_IDENTIFICATION,
    ANC_APPLICATION_OFFLINE_IDENTIFICATION
} anc_application_state_t;

/* Public methods declaration */

void anc_application_init(
    anc_acquisition_t*  h_ancAcquisition,
    uart_receiver_t*    h_uartReceiver,
    uart_transmitter_t* h_uartTransmitter,
    dma_mem2mem_t*      h_dmaMem2mem0,
    dma_mem2mem_t*      h_dmaMem2mem1
);

void anc_application_start(void);

anc_application_state_t anc_application_getState(void);

#endif /* ANC_APPLICATION_H_ */
