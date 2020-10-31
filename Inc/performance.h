/*
 * performance.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef PERFORMANCE_H_
#define PERFORMANCE_H_

#include "main.h"

#define PERFORMANCE_TIMER       TIM3

//#define PERFORMANCE_PERIOD      1024

struct performance_struct
{
    uint16_t    begin;
    uint16_t    result;
    //uint32_t    resultSum;
    //uint32_t    counter;
    //void        (*callback)(uint16_t);
};

typedef volatile struct performance_struct performance_t;

volatile performance_t anc_acq_performance;

static inline void performance_start_timer(void)
{
    LL_TIM_EnableCounter(PERFORMANCE_TIMER);
}

/*
void performance_init(
    performance_t*  self
)
{
    self->counter   = 0;
    self->resultSum = 0;
}
*/

static inline void performance_begin(
    performance_t*  self
)
{
    self->begin = (uint16_t) LL_TIM_GetCounter(PERFORMANCE_TIMER);
}

static inline void performance_end(
    performance_t*  self
)
{
    uint16_t end;

    end = (uint16_t) LL_TIM_GetCounter(PERFORMANCE_TIMER);
    self->result = end - self->begin;
    /*
    uint16_t end;
    uint16_t result;

    end    = (uint16_t) LL_TIM_GetCounter(PERFORMANCE_TIMER);
    result = end - self->begin;

    self->resultSum += result;

    (self->counter)++;
    if (self->counter == PERFORMANCE_PERIOD)
    {
        self->counter = 0;
        self->callback(self->resultSum / PERFORMANCE_PERIOD);
    }
    */
}

static inline uint16_t performance_get_result(
    performance_t*  self
)
{
    return self->result;
}



#endif /* PERFORMANCE_H_ */
