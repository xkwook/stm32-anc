/*
 * anc_processing.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "anc_processing.h"

/* Private data */

static const m_inShiftTable[4] =
{
    1u,
    2u,
    3u,
    4u
};

static const m_outShiftTable[4] =
{
    4u,
    3u,
    2u,
    1u
};

/* Pulic metods definition */

void anc_processing_init(anc_processing_t* self)
{

}

inline anc_processing_preprocessing_data_t anc_processing_preprocessing(
    anc_processing_t*   self,
    uint16_t*           refMicBfr,
    uint16_t*           errMicBfr
)
{
    anc_processing_preprocessing_data_t samples;
    q15_t*      refIn_p;
    q15_t*      errIn_p;
    anc_gain_t  refGain;
    anc_gain_t  errGain;
    uint32_t    refShift;
    uint32_t    errShift;

    /* Load previous gains */
    refGain = anc_gain_refGet();
    errGain = anc_gain_errGet();

    /* Run AGC */
    agc_adapt(self->h_agc, refMicBfr, errMicBfr, outDacBfr);

    /* Init state pointers */
    refIn_p = self->refIn;
    errIn_p = self->errIn;

    /* Convert ref data to q15 buffer */
    refShift = m_inShiftTable[(int) refGain];

    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p++ = (*refMicBfr++ - ANC_PROCESSING_OFFSET)
                << refShift;
    *refIn_p   = (*refMicBfr   - ANC_PROCESSING_OFFSET)
                << refShift;

    /* Convert err data to q15 buffer */
    errShift = m_inShiftTable[(int) errGain];

    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p++ = (*errMicBfr++ - ANC_PROCESSING_OFFSET)
                << errShift;
    *errIn_p   = (*errMicBfr   - ANC_PROCESSING_OFFSET)
                << errShift;

    /* FIR and decimate */
    samples.refSample = fir_circular_decimate_calculate(self->h_fir_decimate_ref);
    samples.errSample = fir_circular_decimate_calculate(self->h_fir_decimate_err);

    /* Perform IIR3 on input data */
    iir3_circular_pushData(self->h_iir3_ref, samples.refSample);
    iir3_circular_pushData(self->h_iir3_err, samples.errSample);
    samples.refSample = iir3_circular_calculate(self->h_iir3_ref);
    samples.errSample = iir3_circular_calculate(self->h_iir3_err);

    /* TODO: Add data to log buffer */
}

inline void anc_processing_postprocessing(
    anc_processing_t*   self,
    q15_t               outSample,
    uint16_t*           outDacBfr
)
{
    q15_t*      out_p;
    anc_gain_t  outGain;
    uint32_t    outShift;

    /* Load new gain for output */
    outGain = anc_gain_outGet();

    /* Init state pointer */
    out_p = self->out;

    /* Interpolate u with FIR */
    fir_circular_interp_pushData(self->h_fir_interp, outSample);
    fir_circular_interp_calculate(self->h_fir_interp, out_p);

    /* Convert from q15 to DAC format with saturation */
    outShift = m_outShiftTable[(int) outGain];

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

    /* TODO: Log data now maybe? */
}