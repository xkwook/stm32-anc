/*
 * state_buffer.c
 *
 *  Created on: Nov 5, 2020
 *      Author: klukomski
 */

#include "state_buffer.h"

/* Public methods definition */

void state_buffer_init(
    state_buffer_t* self,
    volatile float* bfr_p,
    uint32_t        length
)
{
    self->bfr_p     = bfr_p;
    self->length    = length;

    /* Write at first index */
    self->index     = 0;

    for (int i = 0; i < length; i++)
    {
        self->bfr_p[i] = 0.0;
    }
}