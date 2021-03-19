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
#define ANC_OFFLINE_MU_DEFAULT              0.01f
#define ANC_OFFLINE_ALPHA_DEFAULT           1.0f
extern volatile float anc_offline_mu;
extern volatile float anc_offline_alpha;

/* ANC NLMS for FxLMS online adaptation */
#define ANC_ONLINE_MU_DEFAULT               -0.0001f
#define ANC_ONLINE_ALPHA_DEFAULT            0.999999f
extern volatile float anc_online_mu;
extern volatile float anc_online_alpha;

/* Secondary path filter */
//#define ANC_SN_FILTER_LENGTH                1264
#define ANC_SN_FILTER_LENGTH                128
volatile float anc_Sn_coeffs[ANC_SN_FILTER_LENGTH];

/* Feed-forward control adaptive filter */
//#define ANC_WN_FILTER_LENGTH                1264
#define ANC_WN_FILTER_LENGTH                128
volatile float anc_Wn_coeffs[ANC_WN_FILTER_LENGTH];

/* ANC Algorithm filter state buffers */
volatile float anc_Sn_bfr   [ANC_SN_FILTER_LENGTH];
volatile float anc_SnOut_bfr[ANC_WN_FILTER_LENGTH];
volatile float anc_Wn_bfr   [ANC_WN_FILTER_LENGTH];

/* High-pass (20 Hz) IIR filter with notch (50 Hz) */
#define ANC_IIR_FILTER_ORDER                2
/* High-pass (20 Hz) IIR filter */
extern const q15_t anc_iir_hp_b_coeffs[ANC_IIR_FILTER_ORDER + 1];
extern const q15_t anc_iir_hp_a_coeffs[ANC_IIR_FILTER_ORDER];
extern const q31_t anc_iir_hp_scaling_factor;
/* Notch (50 Hz) IIR filter */
extern const q15_t anc_iir_notch_b_coeffs[ANC_IIR_FILTER_ORDER + 1];
extern const q15_t anc_iir_notch_a_coeffs[ANC_IIR_FILTER_ORDER];
extern const q31_t anc_iir_notch_scaling_factor;

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
