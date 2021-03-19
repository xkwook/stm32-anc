/*
 * anc_processing.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef ANC_PROCESSING_H_
#define ANC_PROCESSING_H_

#include "anc_math.h"
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#else
#include "anc_gain.h"
#include "agc.h"
#endif
#include "fir_circular_decimate.h"
#include "fir_circular_interp.h"
#include "iir2_circular.h"
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#else
#include "uart_transmitter.h"
#include "dma_mem2mem.h"
#endif

#define ANC_PROCESSING_CHUNK_SIZE    4
#define ANC_PROCESSING_OFFSET        2048

/* Private data */

extern const uint32_t anc_processing_inShiftTable[4];

extern const uint32_t anc_processing_outShiftTable[4];

typedef volatile struct __attribute__((packed))
{
    q15_t   refSample;
    q15_t   errSample;
    q15_t   outSample;
} anc_processing_logData_t;

struct anc_processing_preprocessing_data_struct
{
    q15_t   refSample;
    q15_t   errSample;
};

typedef struct anc_processing_preprocessing_data_struct
    anc_processing_preprocessing_data_t;

struct anc_processing_struct
{
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#else
    agc_t*                  h_agc;
    uart_transmitter_t*     h_uart_transmitter;
    anc_processing_logData_t*   logData_p;
#endif
    fir_circular_decimate_t fir_decimate_ref;
    fir_circular_decimate_t fir_decimate_err;
    iir2_circular_t         iir2_hp_ref;
    iir2_circular_t         iir2_notch_ref;
    iir2_circular_t         iir2_hp_err;
    iir2_circular_t         iir2_notch_err;
    fir_circular_interp_t   fir_interp;
    q15_t                   refIn[ANC_PROCESSING_CHUNK_SIZE];
    q15_t                   errIn[ANC_PROCESSING_CHUNK_SIZE];
};

typedef volatile struct anc_processing_struct anc_processing_t;

/* Public methods declaration */

void anc_processing_init(
    anc_processing_t*           self0,
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
    anc_processing_t*           self1
#else
    anc_processing_t*           self1,
    agc_t*                      h_agc,
    uart_transmitter_t*         h_uart_transmitter,
    anc_processing_logData_t*   logData_p
#endif
);

__attribute__((weak)) void anc_processing_onErrorCallback(
    anc_processing_t*   self
);

/* Inline methods */

static inline anc_processing_preprocessing_data_t anc_processing_preprocessing_filtering(
    anc_processing_t*   self
)
{
    anc_processing_preprocessing_data_t samples;

    /* FIR and decimate */
    samples.refSample = fir_circular_decimate_calculate(&self->fir_decimate_ref);
    samples.errSample = fir_circular_decimate_calculate(&self->fir_decimate_err);

    /* Perform IIR high pass on input data */
    iir2_circular_pushData(&self->iir2_hp_ref, samples.refSample);
    iir2_circular_pushData(&self->iir2_hp_err, samples.errSample);
    samples.refSample = iir2_circular_calculate(&self->iir2_hp_ref);
    samples.errSample = iir2_circular_calculate(&self->iir2_hp_err);

    /* Perform IIR notch filter on input data */
    /*
    iir2_circular_pushData(&self->iir2_notch_ref, samples.refSample);
    iir2_circular_pushData(&self->iir2_notch_err, samples.errSample);
    samples.refSample = iir2_circular_calculate(&self->iir2_notch_ref);
    samples.errSample = iir2_circular_calculate(&self->iir2_notch_err);
    */

    return samples;
}

static inline anc_processing_preprocessing_data_t anc_processing_preprocessing(
    anc_processing_t*   self,
    volatile uint16_t*  refMicBfr,
    volatile uint16_t*  errMicBfr,
    volatile uint16_t*  outDacBfr
)
{
    anc_processing_preprocessing_data_t samples;
    volatile q15_t*      refIn_p;
    volatile q15_t*      errIn_p;
    //anc_gain_t  refGain;
    //anc_gain_t  errGain;
    //uint32_t    refShift;
    //uint32_t    errShift;

    /* Load previous gains */
    //refGain = anc_gain_refGet();
    //errGain = anc_gain_errGet();

    /* Run AGC */
    //agc_adapt(self->h_agc, refMicBfr, errMicBfr, outDacBfr);

    /* Init state pointers */
    refIn_p = self->refIn;
    errIn_p = self->errIn;

    /* Convert ref data to q15 buffer */
    /*
    refShift = anc_processing_inShiftTable[(int) refGain];

    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p   = (*refMicBfr   - ANC_PROCESSING_OFFSET)
                << refShift;
    */

    /* Convert err data to q15 buffer */
    /*
    errShift = anc_processing_inShiftTable[(int) errGain];

    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p   = (*errMicBfr   - ANC_PROCESSING_OFFSET)
                << errShift;
    */
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *refIn_p   = (*refMicBfr   - ANC_PROCESSING_OFFSET)
                << 4;

    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << 4;
    *errIn_p   = (*errMicBfr   - ANC_PROCESSING_OFFSET)
                << 4;

    /* FIR and decimate */
    samples.refSample = fir_circular_decimate_calculate(&self->fir_decimate_ref);
    samples.errSample = fir_circular_decimate_calculate(&self->fir_decimate_err);

    /* Perform IIR high pass on input data */
    iir2_circular_pushData(&self->iir2_hp_ref, samples.refSample);
    iir2_circular_pushData(&self->iir2_hp_err, samples.errSample);
    samples.refSample = iir2_circular_calculate(&self->iir2_hp_ref);
    samples.errSample = iir2_circular_calculate(&self->iir2_hp_err);

    /* Perform IIR notch filter on input data */
    /*
    iir2_circular_pushData(&self->iir2_notch_ref, samples.refSample);
    iir2_circular_pushData(&self->iir2_notch_err, samples.errSample);
    samples.refSample = iir2_circular_calculate(&self->iir2_notch_ref);
    samples.errSample = iir2_circular_calculate(&self->iir2_notch_err);
    */

    return samples;
}

static inline void anc_processing_postprocessing_filtering(
    anc_processing_t*                   self,
    anc_processing_preprocessing_data_t inSamples,
    q15_t                               outSample,
    q15_t*                              outBfr
)
{
    /* Interpolate u with FIR */
    fir_circular_interp_pushData(&self->fir_interp, outSample);
    fir_circular_interp_calculate(&self->fir_interp, outBfr);
}

static inline void anc_processing_postprocessing(
    anc_processing_t*                   self,
    anc_processing_preprocessing_data_t inSamples,
    q15_t                               outSample,
    volatile uint16_t*                  outDacBfr
)
{
    q15_t       out[ANC_PROCESSING_CHUNK_SIZE];
    q15_t*      out_p = out;
    //anc_gain_t  outGain;
    //uint32_t    outShift;

    /* Load new gain for output */
    //outGain = anc_gain_outGet();

    /* Interpolate u with FIR */
    fir_circular_interp_pushData(&self->fir_interp, outSample);
    fir_circular_interp_calculate(&self->fir_interp, out_p);

    /* Convert from q15 to 12-bit DAC format with saturation */
    /*
    outShift = anc_processing_outShiftTable[(int) outGain];

    *outDacBfr++ = (uint16_t) (
        __SSAT((*out_p++ >> outShift), 12)
        + ANC_PROCESSING_OFFSET);
    *outDacBfr++ = (uint16_t) (
        __SSAT((*out_p++ >> outShift), 12)
        + ANC_PROCESSING_OFFSET);
    *outDacBfr++ = (uint16_t) (
        __SSAT((*out_p++ >> outShift), 12)
        + ANC_PROCESSING_OFFSET);
    *outDacBfr   = (uint16_t) (
        __SSAT((*out_p   >> outShift), 12)
        + ANC_PROCESSING_OFFSET);
    */
    *outDacBfr++ = (*out_p++ >> 4) + ANC_PROCESSING_OFFSET;
    *outDacBfr++ = (*out_p++ >> 4) + ANC_PROCESSING_OFFSET;
    *outDacBfr++ = (*out_p++ >> 4) + ANC_PROCESSING_OFFSET;
    *outDacBfr   = (*out_p   >> 4) + ANC_PROCESSING_OFFSET;
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#else
    if (uart_transmitter_isBusy(self->h_uart_transmitter))
    {
        anc_processing_onErrorCallback(self);
    }
    else
    {
        self->logData_p->refSample = inSamples.refSample;
        self->logData_p->errSample = inSamples.errSample;
        self->logData_p->outSample = outSample;

        uart_transmitter_start(self->h_uart_transmitter);
    }
#endif
}

#endif /* ANC_PROCESSING_H_ */
