/*
 * anc_parameters.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_PARAMETERS_H_
#define ANC_PARAMETERS_H_

#include "anc_math.h"

typedef enum
{
    DISABLED,
    ENABLED
} enable_t;

/* Decimation factor/chunk size in algorithm */
#define ANC_DECIMATION_RATIO                4
#define ANC_CHUNK_SIZE                      ANC_DECIMATION_RATIO

/* Offline identification cycles time - 60 s */
#define ANC_OFFLINE_IDENTIFICATION_CYCLES   (2000u * 60)

/* ANC NLMS offline identification parameters */
#define ANC_OFFLINE_MU                      1.0f
#define ANC_OFFLINE_ALPHA                   ((q15_t) ((1u << 15) - 1))

/* ANC NLMS for FxLMS online adaptation */
#define ANC_ONLINE_MU                       -0.1f
#define ANC_ONLINE_ALPHA                    ((q15_t) 32735)

/* Secondary path filter */
#define ANC_SN_FILTER_LENGTH                64
volatile q15_t anc_Sn_coeffs[ANC_SN_FILTER_LENGTH];

/* Feed-forward control adaptive filter */
#define ANC_WN_FILTER_LENGTH                64
volatile q15_t anc_Wn_coeffs[ANC_WN_FILTER_LENGTH];

/* High-pass (20 Hz) IIR filter with notch (50 Hz) */
#define ANC_IIR_FILTER_ORDER                3
extern const q15_t anc_iir_b_coeffs[ANC_IIR_FILTER_ORDER + 1];
extern const q15_t anc_iir_a_coeffs[ANC_IIR_FILTER_ORDER];
extern const q31_t anc_iir_scaling_factor;

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
