/*
 * ringbuffer.c
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#include "ringbuffer.h"
#include <string.h>

void    ringbuffer_Init(
    ringbuffer_t*   self,
    void*           bfr_p,
    uint32_t        itemSize_u32,
    uint32_t        bfrSize_u32
)
{
    self->bfr_p         = bfr_p;
    self->itemSize_u32  = itemSize_u32;
    self->last_p        = bfr_p + bfrSize_u32 * (itemSize_u32 - 1);
    self->feed_p        = bfr_p;

    /* Init memory to zeros */
    memset(bfr_p, 0, bfrSize_u32 * itemSize_u32);
}

void*   ringbuffer_getPushPtr(
    ringbuffer_t*   self
)
{
    return self->feed_p;
}

void    ringbuffer_turn(
    ringbuffer_t*   self
)
{
    if (self->feed_p == self->last_p)
    {
        /* Skip to first item */
        self->feed_p = self->bfr_p;
    }
    else
    {
        /* Skip to next item */
        self->feed_p += self->itemSize_u32;
    }
}

void ringbuffer_push(ringbuffer_t* self, void* item_p)
{
    /* Put data at feed pointer */
    memcpy(self->feed_p, item_p, self->itemSize_u32);

    ringbuffer_turn(self);
}

void*   ringbuffer_getItemPtr(
    ringbuffer_t    self,
    uint32_t        invIdx_u32
);
{
    void* read_p = self->feed_p - invIdx_u32 * self->itemSize_u32;

    /* If it shoots under beginning of buffer */
    while (read_p < self->bfr_p)
    {
        read_p = self->last_p - (self->bfr_p - read_p);
    }

    return read_p;
}

void    ringbuffer_getItem(
    ringbuffer_t    self,
    uint32_t        invIdx_u32,
    void*           item_p
);
{
    void* read_p = ringbuffer_getPtr(self, invIdx_u32);

    /* Put data at feed pointer */
    memcpy(item_p, read_p, self->itemSize_u32);
}