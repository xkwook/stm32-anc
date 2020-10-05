/*
 * anc_processing.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_processing.h"

#include "anc_parameters.h"

/* Private data */

const uint32_t anc_processing_inShiftTable[4] =
{
    1u,
    2u,
    3u,
    4u
};

const uint32_t anc_processing_outShiftTable[4] =
{
    4u,
    3u,
    2u,
    1u
};

/* Private methods declaration */

void InitProcessing(
    anc_processing_t*           self0,
    anc_processing_t*           self1,
    agc_t*                      h_agc,
    uart_transmitter_t*         h_uart_transmitter,
    anc_processing_logData_t*   logData_p
);

/* Pulic metods definition */

void anc_processing_init(
    anc_processing_t*           self0,
    anc_processing_t*           self1,
    agc_t*                      h_agc,
    uart_transmitter_t*         h_uart_transmitter,
    anc_processing_logData_t*   logData_p
)
{
    /* Init circular filters for both stages */
    InitProcessing(self0, self1,
        h_agc, h_uart_transmitter, logData_p);
    InitProcessing(self1, self0,
        h_agc, h_uart_transmitter, logData_p);

    /* Configure uart transfer */
    uart_transmitter_setMsg(h_uart_transmitter,
        (uint8_t*) logData_p, sizeof(*logData_p));
}

/* Private methods definition */

void InitProcessing(
    anc_processing_t*           self0,
    anc_processing_t*           self1,
    agc_t*                      h_agc,
    uart_transmitter_t*         h_uart_transmitter,
    anc_processing_logData_t*   logData_p
)
{
    /* Init handle pointers */
    self0->h_agc                = h_agc;
    self0->h_uart_transmitter   = h_uart_transmitter;
    self0->logData_p            = logData_p;

    fir_circular_decimate_init(
        &(self0->fir_decimate_ref),
        (q15_t*) anc_fir_decim_coeffs,
        self1->refIn,
        self0->refIn
    );

    fir_circular_decimate_init(
        &(self0->fir_decimate_err),
        (q15_t*) anc_fir_decim_coeffs,
        self1->errIn,
        self0->errIn
    );

    iir3_circular_init(
        &(self0->iir3_ref),
        (q15_t*) anc_iir_b_coeffs,
        (q15_t*) anc_iir_a_coeffs,
        anc_iir_scaling_factor,
        iir3_circular_getDataInPtr(&(self1->iir3_ref)),
        iir3_circular_getDataOutPtr(&(self1->iir3_ref))
    );

    iir3_circular_init(
        &(self0->iir3_err),
        (q15_t*) anc_iir_b_coeffs,
        (q15_t*) anc_iir_a_coeffs,
        anc_iir_scaling_factor,
        iir3_circular_getDataInPtr(&(self1->iir3_err)),
        iir3_circular_getDataOutPtr(&(self1->iir3_err))
    );

    fir_circular_interp_init(
        &(self0->fir_interp),
        (q15_t*) anc_fir_interp_coeffs,
        fir_circular_interp_getDataInPtr(&(self1->fir_interp))
    );
}
