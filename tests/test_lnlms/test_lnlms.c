#include <stdio.h>
#include <string.h>
#include <math.h>
#include "test_vector_handler.h"
#include "fir.h"
#include "lnlms.h"
#include "anc_parameters.h"

#define DEBUG           1

#define TEST_NAME       "test_lnlms"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       1
#define MAX_MSE         0.001

#define LNLMS_ALPHA     32767.0/32768.0
#define LNLMS_MU        0.01

static state_buffer_t   state_buffer_ref;
static fir_t            fir_ref;

static state_buffer_t   state_buffer;
static fir_t            fir;
static lnlms_t          lnlms;

static float stateBfr_ref[ANC_FIR_FILTER_ORDER + 1];

static float stateBfr[ANC_FIR_FILTER_ORDER + 1];

static float wn_coeffs_ref[ANC_FIR_FILTER_ORDER + 1];
static float wn_coeffs[ANC_FIR_FILTER_ORDER + 1];

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

#if DEBUG
    for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
    {
        printf("b%02d = %5d     b%02d_ref = %5d\n", i, (int32_t)(wn_coeffs[i] * 32768.0), i,
            anc_fir_interp_coeffs[i]);
    }
#endif

    /* Check coeffs at the end */
    coeffsError();

    test_vector_handler_deinit(&vectHandler);

    return (error || result);
}

void filtersInit(void)
{
    /* Init reference coeffs */
    for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
    {
        wn_coeffs_ref[i] = (float) anc_fir_interp_coeffs[i] / 32768.0;
    }

    state_buffer_init(
        &state_buffer,
        stateBfr,
        ANC_FIR_FILTER_ORDER + 1
    );

    state_buffer_init(
        &state_buffer_ref,
        stateBfr_ref,
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_init(
        &fir,
        &state_buffer,
        wn_coeffs
    );

    fir_init(
        &fir_ref,
        &state_buffer_ref,
        wn_coeffs_ref
    );

    lnlms_init(
        &lnlms,
        &state_buffer,
        wn_coeffs,
        LNLMS_ALPHA,
        LNLMS_MU
    );

    lnlms_initCoeffs(wn_coeffs, ANC_FIR_FILTER_ORDER + 1);
}

void coeffsError(void)
{
    float mean_error;

    mean_error = 0.0;

    for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
    {
        mean_error += fabs((double)(wn_coeffs[i] - wn_coeffs_ref[i]));
    }
    mean_error /= (ANC_FIR_FILTER_ORDER + 1);

    printf("Mean error for coeffs estimation with LNLMS: %lf\n", mean_error);
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    float out;
    float ref;
    float error;

    float in = (float) input_samples[0] / 32768.0;

    /* Calculate reference */
    state_buffer_pushData(&state_buffer_ref, in);
    ref = fir_calculate(&fir_ref);
    state_buffer_turn(&state_buffer_ref);

    /* Calculate with current filter */
    state_buffer_pushData(&state_buffer, in);
    out = fir_calculate(&fir);

    /* Calculate error */
    error = ref - out;

#if DEBUG
    if (iteration == 56 || iteration == 57)
    {
        printf("Iteration %d:\n", iteration);

        printf("Out: %5f\n", out);

        printf("Error: %5f\n", error);

        printf("State buffer:\n");
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tx%02d = %5f\n", i, stateBfr[i]);
        }

        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tb%02d = %5f\n", i, wn_coeffs[i]);
        }

        printf("Index: %d\n", state_buffer.index);
    }
#endif

    lnlms_update(&lnlms, error);

    /* Turn state buffer */
    state_buffer_turn(&state_buffer);

    float out_err = error * 32768.0;
    if (out_err > 32767.0)
    {
        out_err = 32767.0;
    }
    if (out_err < -32768.0)
    {
        out_err = -32768.0;
    }

    output_samples[0] = (q15_t) (out_err);

#if DEBUG
    if (iteration == 56 || iteration == 57)
    {
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("b%02d = %5f\n", i, wn_coeffs[i]);
        }
    }
#endif
}