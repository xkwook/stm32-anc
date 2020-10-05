#include <stdio.h>
#include <string.h>
#include "test_vector_handler.h"
#include "fir_circular_decimate.h"
#include "anc_parameters.h"

#define DEBUG           0

#define TEST_NAME       "test_fir_circular_decimate"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        4
#define OUT_CHUNK       1
#define MAX_MSE         0.00001

static fir_circular_decimate_t fir[2];

static q15_t samples[2][FIR_CIRCULAR_DECIMATE_CHUNK_SIZE];

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
    fir_circular_decimate_init(
        &fir[0],
        (q15_t*) anc_fir_decim_coeffs,
        &samples[1][0],
        &samples[0][0]
    );

    fir_circular_decimate_init(
        &fir[1],
        (q15_t*) anc_fir_decim_coeffs,
        &samples[0][0],
        &samples[1][0]
    );
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    fir_circular_decimate_t* h_fir;

    /* Get proper filter handler */
    h_fir = &fir[iteration % 2];

    /* Feed data */
    memcpy(&samples[iteration % 2][0], input_samples,
        IN_CHUNK * sizeof(input_samples[0]));

#if DEBUG
    if (iteration < 5)
    {
        for (int i = 0; i < FIR_CIRCULAR_DECIMATE_CHUNK_SIZE; i++)
        {
            printf("\tx%02d = %5d", i, h_fir->dataIn_p[3 - i]);
        }
        printf("\n");
        for (int i = 0; i < FIR_CIRCULAR_DECIMATE_CHUNK_SIZE; i++)
        {
            printf("\tx%02d = %5d", i + FIR_CIRCULAR_DECIMATE_CHUNK_SIZE, h_fir->oldDataIn_p[3 - i]);
        }
        printf("\n");
        for (int i = 0; i < FIR_CIRCULAR_DECIMATE_BFR_SIZE / FIR_CIRCULAR_DECIMATE_CHUNK_SIZE; i++)
        {
            for (int k = 0; k < FIR_CIRCULAR_DECIMATE_CHUNK_SIZE; k++)
            {
                printf("\tx%02d = %5d", i*4 + k + 2*FIR_CIRCULAR_DECIMATE_CHUNK_SIZE, h_fir->stateBfr[23 - i*4 - k]);
            }
            printf("\n");
        }
    }
#endif

    /* Calculate */
    output_samples[0] = fir_circular_decimate_calculate(h_fir);
}