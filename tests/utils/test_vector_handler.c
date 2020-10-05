#include "test_vector_handler.h"

#define SQUARE(x)   ((x) * (x))

int  test_vector_handler_init(
    test_vector_handler_t*  self,
    const char* const       test_name,
    const char* const       input_filename,
    const char* const       output_filename,
    const char* const       expect_filename,
    uint32_t                in_chunk,
    uint32_t                out_chunk,
    float                   max_mse
)
{
    printf("Running test: %s\n", test_name);

    if (    (out_chunk > TEST_VECTOR_HANDLER_MAX_CHUNK)
        ||  (in_chunk > TEST_VECTOR_HANDLER_MAX_CHUNK))
    {
        printf("\tExceeded max chunk size: %u\n", TEST_VECTOR_HANDLER_MAX_CHUNK);
        return -1;
    }
    self->in_chunk  = in_chunk;
    self->out_chunk = out_chunk;

    self->inFile_p  = fopen(input_filename, "r");

    if (self->inFile_p == NULL)
    {
        printf("\tInput file doesn't exist!\n");
        return -1;
    }

    self->outFile_p = fopen(output_filename, "w");

    if (self->outFile_p == NULL)
    {
        printf("\tOutput file problem!\n");
        return -1;
    }

    self->expectFile_p = fopen(expect_filename, "r");

    if (self->expectFile_p == NULL)
    {
        printf("\tExpect file doesn't exist!\n");
        return -1;
    }

    self->max_mse = max_mse;

    return 0;
}

void test_vector_handler_deinit(
    test_vector_handler_t*  self
)
{
    fclose(self->inFile_p);
    fclose(self->outFile_p);
    fclose(self->expectFile_p);
}

int  test_vector_handler_run(
    test_vector_handler_t*  self
)
{
    int result = 0;

    float in_f;
    float out_f;
    float expect_f;

    float mse = 0;

    int i = 0;
    int user_i = 0;

    while (fscanf(self->inFile_p, "%f\n", &in_f) == 1)
    {
        /* Transform float to q15 format */
        self->in[i % self->in_chunk] = (q15_t) (in_f * 32768);

        if (i % self->in_chunk == (self->in_chunk - 1))
        {
            /* Get output data from user callback */
            test_vector_handler_calculation_callback(
                self, self->in, self->out, user_i);
            user_i++;

            /* Calculate outputs */
            for (int k = 0; k < self->out_chunk; k++)
            {
                /* Transform output to float */
                out_f = ((float) self->out[k]) / 32768.0;

                /* Read expect sample */
                if (fscanf(self->expectFile_p, "%f\n", &expect_f) != 1)
                {
                    printf("No proper length expectation file!\n");
                    return -1;
                }

                /* Calculate mean square error */
                mse += SQUARE(out_f - expect_f);

                /* Save to file */
                fprintf(self->outFile_p, "%f\n", out_f);
            }
        }

        /* Loop counter */
        i++;
    }

    mse /= (i * self->out_chunk / self->in_chunk);

    printf("\tMean square error: %3.4f\n", mse);

    if (mse > self->max_mse)
    {
        printf("\tTEST FAILED: MSE over maximum!\n");
        result = -1;
    }
    else
    {
        printf("\tTEST PASSED!\n");
    }

    return result;
}