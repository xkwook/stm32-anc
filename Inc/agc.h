/*
 * agc.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef AGC_H_
#define AGC_H_

#include <stdint.h>

#define AGC_CHUNK_SIZE      4

#define AGC_HORIZON         200

/* Gains 2, 4, 10, 20 */

typedef enum {
    AGC_GAIN_2,
    AGC_GAIN_4,
    AGC_GAIN_10,
    AGC_GAIN_20
} agc_gain_t;

struct agc_struct
{
    uint32_t    counter;
    uint16_t    min_u16;
    uint16_t    max_u16;
};

typedef struct agc_struct agc_t;

void agc_init(agc_t* self)
{
    self->counter = 0;
    self->min_u16 = 2048;
    self->max_u16 = 2048;
}

void agc_adapt(agc_t* self, uint16_t* newDataChunk_u16)
{
    for (uint32_t i = 0; i < AGC_CHUNK_SIZE; i++)
    {
        if (newDataChunk_u16[i] > self->max_u16)
        {
            self->max_u16 = newDataChunk_u16[i];
        }
    }
}

#endif /* AGC_H_ */
