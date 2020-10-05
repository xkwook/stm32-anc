#include <stdio.h>
#include "test_vector_handler.h"
#include "iir3_circular.h"
#include "anc_parameters.h"

#define TEST_NAME       "test_iir3_circular"

#define INPUT_FILENAME  TEST_NAME "/input_vector.csv"
#define OUTPUT_FILENAME TEST_NAME "/output_vector.csv"
#define EXPECT_FILENAME TEST_NAME "/expect_vector.csv"

#define IN_CHUNK        1
#define OUT_CHUNK       1
#define MAX_MSE         0.005

static iir3_circular_t iir3[2];

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
    iir3_circular_init(
        &iir3[0],
        (q15_t*) anc_iir_b_coeffs,
        (q15_t*) anc_iir_a_coeffs,
        anc_iir_scaling_factor,
        iir3_circular_getDataInPtr(&iir3[1]),
        iir3_circular_getDataOutPtr(&iir3[1])
    );

    iir3_circular_init(
        &iir3[1],
        (q15_t*) anc_iir_b_coeffs,
        (q15_t*) anc_iir_a_coeffs,
        anc_iir_scaling_factor,
        iir3_circular_getDataInPtr(&iir3[0]),
        iir3_circular_getDataOutPtr(&iir3[0])
    );
}

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
)
{
    iir3_circular_t* h_iir3;

    /* Get proper filter handler */
    h_iir3 = &iir3[iteration % 2];

    iir3_circular_pushData(h_iir3, input_samples[0]);
    output_samples[0] = iir3_circular_calculate(h_iir3);
}