#include <stdio.h>
#include "test_vector_handler.h"
#include "fir_circular_interp.h"
#include "anc_parameters.h"

#define DEBUG           0

#define TEST_NAME       "test_fir_circular_interp"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       4
#define MAX_MSE         0.00001

static fir_circular_interp_t fir[2];

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
    fir_circular_interp_init(
        &fir[0],
        (q15_t*) anc_fir_interp_coeffs,
        fir_circular_interp_getDataInPtr(&fir[1])
    );

    fir_circular_interp_init(
        &fir[1],
        (q15_t*) anc_fir_interp_coeffs,
        fir_circular_interp_getDataInPtr(&fir[0])
    );
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    fir_circular_interp_t* h_fir;

    /* Get proper filter handler */
    h_fir = &fir[iteration % 2];

    /* Feed data */
    fir_circular_interp_pushData(h_fir, input_samples[0]);

#if DEBUG
    if (iteration < 5)
    {
        printf("Before calculation:\n");
        printf("\tx0 = %5d\n", h_fir->dataIn);
        printf("\tx1 = %5d\n", *(h_fir->oldDataIn_p));
        for (int i = 0; i < FIR_CIRCULAR_INTERP_BFR_SIZE; i++)
        {
            printf("\tx%d = %5d\n", i + 2, h_fir->stateBfr[i]);
        }
    }
#endif

    /* Calculate */
    fir_circular_interp_calculate(h_fir, output_samples);

#if DEBUG
    if (iteration < 5)
    {
        printf("After calculation:\n");
        printf("\tx0 = %5d\n", h_fir->dataIn);
        printf("\tx1 = %5d\n", *(h_fir->oldDataIn_p));
        for (int i = 0; i < FIR_CIRCULAR_INTERP_BFR_SIZE; i++)
        {
            printf("\tx%d = %5d\n", i + 2, h_fir->stateBfr[i]);
        }
    }
#endif
#if DEBUG
    if (iteration < 5)
    {
        for (int i = 0; i < FIR_CIRCULAR_INTERP_CHUNK_SIZE; i++)
        {
            printf("\ty%d = %5d\n", i, output_samples[i]);
        }
    }
#endif
}