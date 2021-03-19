#include <stdio.h>
#include <string.h>
#include "test_vector_handler.h"
//#include "fir_circular.h"
#include "fir.h"
#include "anc_parameters.h"

#define DEBUG           0

#define TEST_NAME       "test_fir_circular"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       1
#define MAX_MSE         0.00001

//static fir_circular_t fir[2];
static state_buffer_t stateBfr;
static fir_t fir;

static volatile float bfr[ANC_FIR_FILTER_ORDER + 1];

static volatile float coeffs[ANC_FIR_FILTER_ORDER + 1];

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
    for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
    {
        coeffs[i] = q15_to_float(anc_fir_decim_coeffs[i]);
    }

    state_buffer_init(
        &stateBfr,
        bfr,
        ANC_FIR_FILTER_ORDER + 1
    );

    fir_init(
        &fir,
        &stateBfr,
        coeffs
    );
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    float in, out;

    in = q15_to_float(input_samples[0]);

    state_buffer_pushData(&stateBfr, in);

#if DEBUG
    if (iteration < 5)
    {
        for (int i = 0; i < ANC_FIR_FILTER_ORDER + 1; i++)
        {
            printf("\tx%02d = %5d\n", i, stateBfr[iteration % 2][i]);
        }
    }
#endif

    out = fir_calculate(&fir);

    state_buffer_turn(&stateBfr);

    output_samples[0] = float_to_q15(out);

#if DEBUG
    if (iteration < 5)
    {
        printf("\ty   = %5d\n\n", output_samples[0]);
    }
#endif
}