/*
 * anc_parameters.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_PARAMETERS_H_
#define ANC_PARAMETERS_H_

#include <stdint.h>

/* High-pass (20 Hz) IIR filter with notch (50 Hz) */
#define ANC_IIR_FILTER_ORDER            3
extern const float anc_iir_b_coeffs[ANC_IIR_FILTER_ORDER + 1];
extern const float anc_iir_a_coeffs[ANC_IIR_FILTER_ORDER];

/* Low-pass FIR filter for decimation and interpolation */
#define ANC_FIR_FILTER_ORDER            31
extern const float anc_fir_b_coeffs[ANC_FIR_FILTER_ORDER + 1];

/* Excitation signal flat frequency response for 8kHz 12-bit DAC */
#define ANC_EXCITATION_SIGNAL_LENGTH    8000
extern const uint16_t anc_excitationSignal[ANC_EXCITATION_SIGNAL_LENGTH];

#endif /* ANC_PARAMETERS_H_ */