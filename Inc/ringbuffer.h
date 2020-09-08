/*
 * ringbuffer.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#include <stdint.h>

struct ringbuffer_struct {
    void*       bfr_p;
    uint32_t    itemSize_u32;
    void*       last_p;
    void*       feed_p;
};

typedef struct ringbuffer_struct ringbuffer_t;

void    ringbuffer_init(
    ringbuffer_t*   self,
    void*           bfr_p,
    uint32_t        itemSize_u32,
    uint32_t        bfrSize_u32
);

void*   ringbuffer_getPushPtr(
    ringbuffer_t*   self
);

void    ringbuffer_turn(
    ringbuffer_t*   self
);

void    ringbuffer_push(
    ringbuffer_t*   self,
    void*           item_p
);

void*   ringbuffer_getItemPtr(
    ringbuffer_t*   self,
    uint32_t        invIdx_u32
);

void    ringbuffer_getItem(
    ringbuffer_t*   self,
    uint32_t        invIdx_u32,
    void*           item_p
);

#endif /* RINGBUFFER_H_ */
