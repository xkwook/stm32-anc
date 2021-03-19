#include <stdio.h>
#include <string.h>
#include <math.h>
#include "test_vector_handler.h"
#include "fir.h"
#include "lnlms.h"
#include "anc_parameters.h"
#include "anc_processing.h"
#include "anc_algorithm.h"
#include "lab_model.h"
#include "anc_offline_identification.h"

#define DEBUG           0

#define TEST_NAME       "test_fxlms"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        4
#define OUT_CHUNK       4
#define MAX_MSE         0.001

#define IDENT_ALPHA     1.0f
#define IDENT_MU        0.01f
#define IDENT_CYCLES    60*2000u
//#define IDENT_CYCLES    61
#define ENABLE_PERIOD
#define FXLMS_ALPHA     0.999999f
#define FXLMS_MU        -0.0001f

anc_processing_t anc_proc[2];

anc_algorithm_t anc_algorithm;

anc_offline_identification_t identification;

void filtersInit(void);

void coeffsError(void);

int main(void)
{
    int error;
    int result;
    test_vector_handler_t vectHandler;

    error = test_vector_handler_init(&vectHandler,
        TEST_NAME,
        INPUT_FILENAME,
        OUTPUT_FILENAME,
        EXPECT_FILENAME,
        IN_CHUNK,
        OUT_CHUNK,
        MAX_MSE
    );

    if (!error)
    {
        filtersInit();

        result = test_vector_handler_run(&vectHandler);
    }

    test_vector_handler_deinit(&vectHandler);

    return (error || result);
}

void PrintCoeffs(volatile float* coeffs_p, uint32_t length)
{
    for (int i = 0; i < length / 8; i++)
    {
        printf("% 8.5f % 8.5f % 8.5f % 8.5f % 8.5f % 8.5f % 8.5f % 8.5f\n",
            coeffs_p[8*i + 0],
            coeffs_p[8*i + 1],
            coeffs_p[8*i + 2],
            coeffs_p[8*i + 3],
            coeffs_p[8*i + 4],
            coeffs_p[8*i + 5],
            coeffs_p[8*i + 6],
            coeffs_p[8*i + 7]
        );
    }
}

void filtersInit(void)
{
    anc_algorithm_init(
        &anc_algorithm,
        FXLMS_MU,
        FXLMS_ALPHA
    );

    anc_offline_identification_init(
        &identification,
        IDENT_CYCLES,
        IDENT_MU,
        IDENT_ALPHA
    );

    anc_processing_init(
        &anc_proc[0],
        &anc_proc[1]
    );

    lab_model_init();

    lnlms_initCoeffs(anc_Sn_coeffs, ANC_SN_FILTER_LENGTH);
    lnlms_initCoeffs(anc_Wn_coeffs, ANC_WN_FILTER_LENGTH);

    printf("Initialization done!\n");
}

void anc_offline_identification_onEndCallback(
    anc_offline_identification_t* self
)
{
    printf("Identification done!\n");
    PrintCoeffs(anc_Sn_coeffs, ANC_SN_FILTER_LENGTH);

}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    static q15_t refMic_q15[4] = {0};
    static q15_t errMic_q15[4] = {0};
    static q15_t outDac_q15[4] = {0};

#if DEBUG
    printf("Loop: %4d\n", iteration);
#endif

    anc_processing_t* h_anc_proc;
    anc_algorithm_t*  h_anc_algorithm = &anc_algorithm;

    anc_processing_preprocessing_data_t inputSamples;

    float noise;
    float cancel;

    q15_t out;

    /* Model simulation part */
    for (int i = 0; i < 4; i++)
    {
        float tmp_noise, tmp_cancel;

        noise  = q15_to_float(input_samples[i]);
        cancel = q15_to_float(outDac_q15[i]);

        /* Reference microphone simulation */
        state_buffer_pushData(&(lab_model.stateBfr_noise_ref), noise);
        tmp_noise = fir_calculate(&(lab_model.fir_noise_ref));
        state_buffer_turn(&(lab_model.stateBfr_noise_ref));

        state_buffer_pushData(&(lab_model.stateBfr_cancel_ref), cancel);
        tmp_cancel = fir_calculate(&(lab_model.fir_cancel_ref));
        state_buffer_turn(&(lab_model.stateBfr_cancel_ref));

        refMic_q15[i] = float_to_q15(tmp_noise + tmp_cancel);

        /* Error microphone simulation */
        state_buffer_pushData(&(lab_model.stateBfr_noise_err), noise);
        tmp_noise = fir_calculate(&(lab_model.fir_noise_err));
        state_buffer_turn(&(lab_model.stateBfr_noise_err));

        state_buffer_pushData(&(lab_model.stateBfr_cancel_err), cancel);
        tmp_cancel = fir_calculate(&(lab_model.fir_cancel_err));
        state_buffer_turn(&(lab_model.stateBfr_cancel_err));

        errMic_q15[i] = float_to_q15(tmp_noise + tmp_cancel) * 0.5;
    }

    h_anc_proc = &anc_proc[iteration % 2];

    /* Feed data */
    for (int i = 0; i < 4; i++)
    {
        h_anc_proc->refIn[i] = refMic_q15[i];
        h_anc_proc->errIn[i] = errMic_q15[i];
    }
    inputSamples = anc_processing_preprocessing_filtering(
        h_anc_proc
    );
    //anc_algorithm_enable(h_anc_algorithm);
    if (iteration < IDENT_CYCLES)
    {
        out = anc_offline_identification_calculate(
            &identification,
            inputSamples
        );
        //output_samples[0] = inputSamples.refSample;
    }
    else
    {
        if (iteration == IDENT_CYCLES + 2000u*40)
        {
            anc_algorithm_enable(h_anc_algorithm);
            for (int i = 0; i < ANC_SN_FILTER_LENGTH; i++)
            {
                /* Clear Sn buffer */
                anc_Sn_bfr[i] = 0.0;
                anc_SnOut_bfr[i] = 0.0;
                anc_Wn_bfr[i] = 0.0;
            }
        }

        out = anc_algorithm_calculate(
            h_anc_algorithm,
            inputSamples
        );
        /* Save error microphone decimated data as output */
        //output_samples[0] = inputSamples.errSample;
    }
#if DEBUG
    printf("ref: %5d err: %5d out: %5d\n", inputSamples.refSample, inputSamples.errSample, out);
#endif
    anc_processing_postprocessing_filtering(
        h_anc_proc,
        inputSamples,
        out,
        outDac_q15
    );

    /* Save identification error as output */
    output_samples[0] = errFiltered2;
    output_samples[1] = inputSamples.refSample;
    output_samples[2] = inputSamples.errSample;
    output_samples[3] = out;
}

