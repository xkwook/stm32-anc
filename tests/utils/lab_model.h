#ifndef LAB_MODEL_H_
#define LAB_MODEL_H_

#include "state_buffer.h"
#include "fir.h"

#define LAB_MODEL_LENGTH        2048

extern volatile float coeffs_noise_ref     [LAB_MODEL_LENGTH];
extern volatile float coeffs_noise_err     [LAB_MODEL_LENGTH];
extern volatile float coeffs_cancel_ref    [LAB_MODEL_LENGTH];
extern volatile float coeffs_cancel_err    [LAB_MODEL_LENGTH];

volatile float state_noise_ref      [LAB_MODEL_LENGTH];
volatile float state_noise_err      [LAB_MODEL_LENGTH];
volatile float state_cancel_ref     [LAB_MODEL_LENGTH];
volatile float state_cancel_err     [LAB_MODEL_LENGTH];

struct lab_model_struct
{
    state_buffer_t  stateBfr_noise_ref;
    state_buffer_t  stateBfr_noise_err;
    state_buffer_t  stateBfr_cancel_ref;
    state_buffer_t  stateBfr_cancel_err;
    fir_t           fir_noise_ref;
    fir_t           fir_noise_err;
    fir_t           fir_cancel_ref;
    fir_t           fir_cancel_err;
} lab_model;

void lab_model_init();

#endif