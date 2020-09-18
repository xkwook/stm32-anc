/*
 * anc_parameters.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_PARAMETERS_H_
#define ANC_PARAMETERS_H_

#include "anc_math.h"

/* High-pass (20 Hz) IIR filter with notch (50 Hz) */
#define ANC_IIR_FILTER_ORDER                3
extern const q15_t anc_iir_b_coeffs[ANC_IIR_FILTER_ORDER + 1];
extern const q15_t anc_iir_a_coeffs[ANC_IIR_FILTER_ORDER];

/* Low-pass FIR filters for decimation and interpolation */
#define ANC_FIR_FILTER_ORDER                31
extern const q15_t anc_fir_decim_coeffs[ANC_FIR_FILTER_ORDER + 1];
extern const q15_t anc_fir_interp_coeffs[ANC_FIR_FILTER_ORDER + 1];

/* Excitation signal flat frequency response for 2kHz for LMS identification */
#define ANC_LMS_EXCITATION_SIGNAL_LENGTH    2000
extern const q15_t anc_lms_excitationSignal[ANC_LMS_EXCITATION_SIGNAL_LENGTH];

/* Excitation signal flat frequency response for 8kHz 12-bit DAC */
#define ANC_EXCITATION_SIGNAL_LENGTH        8000
extern const uint16_t anc_excitationSignal[ANC_EXCITATION_SIGNAL_LENGTH];

#endif /* ANC_PARAMETERS_H_ */