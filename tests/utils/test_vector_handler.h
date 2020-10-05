#ifndef TEST_VECTOR_HANDLER_H_
#define TEST_VECTOR_HANDLER_H_

#include <stdio.h>
#include "anc_math.h"

#define TEST_VECTOR_HANDLER_MAX_CHUNK   8

typedef struct
{
    FILE        *inFile_p;
    FILE        *outFile_p;
    FILE        *expectFile_p;
    uint32_t    in_chunk;
    uint32_t    out_chunk;
    float       max_mse;
    q15_t       in[TEST_VECTOR_HANDLER_MAX_CHUNK];
    q15_t       out[TEST_VECTOR_HANDLER_MAX_CHUNK];
} test_vector_handler_t;


int  test_vector_handler_init(
    test_vector_handler_t*  self,
    const char* const       test_name,
    const char* const       input_filename,
    const char* const       output_filename,
    const char* const       expect_filename,
    uint32_t                in_chunk,
    uint32_t                out_chunk,
    float                   max_mse
);

void test_vector_handler_deinit(
    test_vector_handler_t*  self
);

int  test_vector_handler_run(
    test_vector_handler_t*  self
);

void test_vector_handler_calculation_callback(
    test_vector_handler_t*  self,
    q15_t*                  input_samples,
    q15_t*                  output_samples,
    int                     iteration
);

#endif /* TEST_VECTOR_HANDLER_H_ */