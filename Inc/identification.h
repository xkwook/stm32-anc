/*
 * identification.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef IDENTIFICATION_H_
#define IDENTIFICATION_H_

#include "anc_acquisition.h"

#define IDENTIFICATION_BFR_LENGTH   8000

struct identification_struct
{
    anc_acquisition_t*  h_ancAcquisition;
    const uint16_t*     excitationSignal;
    uint32_t    done;
    uint32_t    stabilizingCycles;
    uint32_t    sumCycles;
    uint32_t    refMicSum[IDENTIFICATION_BFR_LENGTH];
    uint32_t    errMicSum[IDENTIFICATION_BFR_LENGTH];
};

typedef volatile struct identification_struct identification_t;

void identification_init(
    identification_t*   self,
    anc_acquisition_t*  h_ancAcquisition,
    const uint16_t*     excitationSignal
);

void identification_configure(
    identification_t*   self,
    uint32_t            stabilizingCycles,
    uint32_t            sumCycles
);

void identification_start(identification_t* self);

uint32_t identification_isDone(identification_t* self);

__attribute__((weak)) void identification_onDoneCallback(volatile uint32_t* refMicSum, volatile uint32_t* errMicSum);

#endif /* IDENTIFICATION_H_ */
