#include <stdio.h>
#include <string.h>
#include "test_vector_handler.h"
#include "fir_circular.h"
#include "anc_parameters.h"

#define DEBUG           0

#define TEST_NAME       "test_fir_circular"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       1
#define MAX_MSE         0.00001

static fir_circular_t fir[2];

static q15_t stateBfr[2][ANC_FIR_FILTER_ORDER + 1];

void filtersInit(void);

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

void filtersInit(void)
{
    fir_circular_init(
        &fir[0],
        (q15_t*) anc_fir_decim_coeffs,
        fir_circular_getDataInPtr(&fir[1]),
        &stateBfr[0][0],
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_circular_init(
        &fir[1],
        (q15_t*) anc_fir_decim_coeffs,
        fir_circular_getDataInPtr(&fir[0]),
        &stateBfr[1][0],
        ANC_FIR_FILTER_ORDER + 1
    );
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    fir_circular_t* h_fir;

    /* Get proper filter handler */
    h_fir = &fir[iteration % 2];

    fir_circular_pushData(h_fir, input_samples[0]);

#if DEBUG
    if (iteration < 5)
    {
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tx%02d = %5d\n", i, stateBfr[iteration % 2][i]);
        }
    }
#endif

    output_samples[0] = fir_circular_calculate(h_fir);

#if DEBUG
    if (iteration < 5)
    {
        printf("\ty   = %5d\n\n", output_samples[0]);
    }
#endif

    /* Move memory by factor of 2 */
    memmove(&stateBfr[iteration % 2][0],
            &stateBfr[iteration % 2][2],
            (ANC_FIR_FILTER_ORDER - 1) * sizeof(stateBfr[0][0]));
}