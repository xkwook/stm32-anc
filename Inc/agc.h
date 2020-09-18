/*
 * agc.h
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#ifndef AGC_H_
#define AGC_H_

#include "anc_gain.h"

#include <stdint.h>


#define AGC_CHUNK_SIZE      4
#define AGC_HORIZON         2000


struct agc_channel_struct
{
    uint16_t    ampMargin;
};

typedef struct agc_channel_struct agc_channel_t;

struct agc_struct
{
    uint32_t        enable;
    uint32_t        counter;
    agc_channel_t   refChannel;
    agc_channel_t   errChannel;
    agc_channel_t   outChannel;
};

typedef struct agc_struct agc_t;


/* Public methods declaration */

void agc_init(agc_t* self);

void agc_enable(agc_t* self);

void agc_disable(agc_t* self);

inline void agc_adapt(
    agc_t*      self,
    uint16_t*   refData_p,
    uint16_t*   errData_p,
    uint16_t*   outData_p
);

#endif /* AGC_H_ */
