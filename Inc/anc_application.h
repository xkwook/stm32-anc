/*
 * anc_application.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_APPLICATION_H_
#define ANC_APPLICATION_H_

#include <stddef.h>

#include "anc_acquisition.h"
#include "identification.h"
#include "uart_receiver.h"
#include "uart_transmitter.h"
//#include "agc.h"  /* now excluded */

typedef enum
{
    ANC_APPLICATION_IDLE,
    ANC_APPLICATION_ACQUISITION,
    ANC_APPLICATION_IDENTIFICATION,
    ANC_APPLICATION_OFFLINE_IDENTIFICATION
} anc_application_state_t;

typedef enum
{
    ANC_APPLICATION_IDENTIFICATION_EMPTY,
    ANC_APPLICATION_IDENTIFICATION_READY
} anc_application_identificationState_t;

struct anc_application_struct
{
    anc_application_state_t state;
    anc_acquisition_t*      h_ancAcquisition;
    uart_receiver_t*        h_uartReceiver;
    uart_transmitter_t*     h_uartTransmitter;
    identification_t*       h_identification;
    anc_application_identificationState_t   identificationState;
};

typedef struct anc_application_struct anc_application_t;

/* Public methods declaration */

void anc_application_init(
    anc_application_t*  self,
    anc_acquisition_t*  h_ancAcquisition,
    uart_receiver_t*    h_uartReceiver,
    uart_transmitter_t* h_uartTransmitter,
    identification_t*   h_identification
);

void anc_application_start(anc_application_t* self);

anc_application_state_t anc_application_getState(anc_application_t* self);

#endif /* ANC_APPLICATION_H_ */
