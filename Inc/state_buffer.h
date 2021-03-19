/*
 * state_buffer.h
 *
 *  Created on: Nov 5, 2020
 *      Author: klukomski
 */

#ifndef STATE_BUFFER_H_
#define STATE_BUFFER_H_

#include "anc_math.h"

struct state_buffer_struct
{
    volatile float* bfr_p;
    uint32_t        index;
    uint32_t        length;
};

typedef volatile struct state_buffer_struct state_buffer_t;

typedef struct
{
    volatile float* ptr;
    uint32_t        length;
} state_buffer_chunk_t;

/* Public methods declaration */

void state_buffer_init(
    state_buffer_t* self,
    volatile float* bfr_p,
    uint32_t        length
);

static inline void state_buffer_pushData(
    state_buffer_t* self,
    float           dataIn
)
{
    uint32_t index;

    /* Load index */
    index = self->index;

    /* Put new data to state buffer */
    self->bfr_p[index] = dataIn;
}

static inline state_buffer_chunk_t state_buffer_firstChunk(
    state_buffer_t* self
)
{
    state_buffer_chunk_t retData;
    uint32_t index;

    /* Load index */
    index = self->index;

    retData.ptr     = &(self->bfr_p[index]);
    retData.length  = index + 1;

    return retData;
}

static inline state_buffer_chunk_t state_buffer_secondChunk(
    state_buffer_t* self
)
{
    state_buffer_chunk_t retData;
    uint32_t index;
    uint32_t end;

    /* Load index */
    index = self->index;

    /* Load length */
    end = self->length - 1;

    retData.ptr     = &(self->bfr_p[end]);
    retData.length  = end - index;

    return retData;
}

static inline void state_buffer_turn(
    state_buffer_t* self
)
{
    uint32_t index;

    /* Load index */
    index = self->index;

    index++;
    /* Shift to beginning if index is overlapping bfr */
    if (index == self->length)
    {
        index = 0;
    }

    /* Save index */
    self->index = index;
}

#endif /* STATE_BUFFER_H_ */
