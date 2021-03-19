#include <stdio.h>
#include <string.h>
#include <math.h>
#include "test_vector_handler.h"
#include "fir_circular.h"
#include "lnlms_circular.h"
#include "anc_parameters.h"

#define DEBUG           1

#define TEST_NAME       "test_lnlms_circular"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       1
#define MAX_MSE         0.001

#define LNLMS_ALPHA     32767.0/32768.0
#define LNLMS_MU        0.01

static fir_circular_t fir_ref[2];

static fir_circular_t fir[2];
static lnlms_circular_t lnlms[2];

static q15_t stateBfr_ref[2][ANC_FIR_FILTER_ORDER + 1];

static q15_t stateBfr[2][ANC_FIR_FILTER_ORDER + 1];

static q15_t wn_coeffs[ANC_FIR_FILTER_ORDER + 1];

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
        printf("b%02d = %5d     b%02d_ref = %5d\n", i, wn_coeffs[i],
            i, anc_fir_interp_coeffs[i]);
    }
#endif

    /* Check coeffs at the end */
    coeffsError();

    test_vector_handler_deinit(&vectHandler);

    return (error || result);
}

void filtersInit(void)
{
    q15_t alpha = (q15_t) (LNLMS_ALPHA * 32768.0);

    fir_circular_init(
        &fir[0],
        wn_coeffs,
        fir_circular_getDataInPtr(&fir[1]),
        &stateBfr[0][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_circular_init(
        &fir[1],
        wn_coeffs,
        fir_circular_getDataInPtr(&fir[0]),
        &stateBfr[1][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_circular_init(
        &fir_ref[0],
        (q15_t*) anc_fir_interp_coeffs,
        fir_circular_getDataInPtr(&fir_ref[1]),
        &stateBfr_ref[0][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_circular_init(
        &fir_ref[1],
        (q15_t*) anc_fir_interp_coeffs,
        fir_circular_getDataInPtr(&fir_ref[0]),
        &stateBfr_ref[1][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    lnlms_circular_init(
        &lnlms[0],
        wn_coeffs,
        alpha,
        LNLMS_MU,
        &stateBfr[0][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    lnlms_circular_init(
        &lnlms[1],
        wn_coeffs,
        alpha,
        LNLMS_MU,
        &stateBfr[1][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    lnlms_circular_initCoeffs(wn_coeffs, ANC_FIR_FILTER_ORDER + 1);
}

void coeffsError(void)
{
    float mean_error;

    mean_error = 0.0;

    for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
    {
        mean_error += fabs((double)(wn_coeffs[i] - anc_fir_interp_coeffs[i]));
    }
    mean_error /= (ANC_FIR_FILTER_ORDER + 1) * 32768.0;

    printf("Mean error for coeffs estimation with LNLMS: %lf\n", mean_error);
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    lnlms_circular_t* h_lnlms;
    fir_circular_t* h_fir;
    fir_circular_t* h_fir_ref;
    q15_t out;
    q15_t ref;
    q31_t error;

    /* Get proper filter handler */
    h_lnlms   = &lnlms[iteration % 2];
    h_fir     = &fir[iteration % 2];
    h_fir_ref = &fir_ref[iteration % 2];

    fir_circular_pushData(h_fir, input_samples[0]);
    fir_circular_pushData(h_fir_ref, input_samples[0]);

    out = fir_circular_calculate(h_fir);
    ref = fir_circular_calculate(h_fir_ref);

    error = (q31_t)ref - (q31_t)out;

#if DEBUG
    if (iteration < 5)
    {
        printf("Iteration %d:\n", iteration);

        printf("Error: %5d\n", error);

        printf("State buffer:\n");
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tx%02d = %5d\n", i, stateBfr[iteration % 2][i]);
        }

        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tb%02d = %5d\n", i, wn_coeffs[i]);
        }
    }
#endif

    lnlms_circular_update(h_lnlms, error);

    output_samples[0] = error;

#if DEBUG
    if (iteration < 5)
    {
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("b%02d = %5d\n", i, wn_coeffs[i]);
        }
    }
#endif

    /* Move memory by factor of 2 of both FIRs */
    memmove(&stateBfr[iteration % 2][0],
            &stateBfr[iteration % 2][2],
            (ANC_FIR_FILTER_ORDER - 1) * sizeof(stateBfr[0][0]));

    memmove(&stateBfr_ref[iteration % 2][0],
            &stateBfr_ref[iteration % 2][2],
            (ANC_FIR_FILTER_ORDER - 1) * sizeof(stateBfr_ref[0][0]));
}