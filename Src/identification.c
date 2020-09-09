/*
 * identification.c
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#include "identification.h"

#define IDENTIFICATION_CHUNK_SIZE   (IDENTIFICATION_BFR_LENGTH / 2)

static identification_t* m_h_identification;

/* Private methods declaration */

static void halfBfrCallback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr);
static void fullBfrCallback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr);

/* Public methods definition */

void identification_init(
    identification_t*   self,
    anc_acquisition_t*  h_ancAcquisition,
    const uint16_t*     excitationSignal
)
{
    m_h_identification      = self;
    self->done              = 0;
    self->h_ancAcquisition  = h_ancAcquisition;
    self->excitationSignal  = (uint16_t*)excitationSignal;
}

void identification_configure(
    identification_t*   self,
    uint32_t            stabilizingCycles,
    uint32_t            sumCycles
)
{
    self->stabilizingCycles = stabilizingCycles;
    self->sumCycles         = sumCycles;
    self->done              = 0;

    anc_acquisition_configure(self->h_ancAcquisition,
        IDENTIFICATION_CHUNK_SIZE,
        halfBfrCallback, fullBfrCallback
    );

    /* Set exctiation signal to DAC buffer */
    for (uint32_t i = 0; i < IDENTIFICATION_BFR_LENGTH; i++)
    {
        self->h_ancAcquisition->outDacBfr[i] = self->excitationSignal[i];
    }
}

void identification_start(identification_t* self)
{
    anc_acquisition_start(self->h_ancAcquisition);
}

uint32_t identification_isDone(identification_t* self)
{
    return self->done;
}

/* Private methods definition */

static void halfBfrCallback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr)
{
    if (m_h_identification->stabilizingCycles == 0)
    {
        /* Calculate sum across buffers */
        for (uint32_t i = 0; i < IDENTIFICATION_CHUNK_SIZE; i++)
        {
            m_h_identification->refMicSum[i] = (uint32_t) refMicBfr[i];
            m_h_identification->errMicSum[i] = (uint32_t) errMicBfr[i];
        }
    }
}

static void fullBfrCallback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr)
{
    if (m_h_identification->stabilizingCycles)
    {
        m_h_identification->stabilizingCycles--;
    }
    else
    {
        /* Calculate sum across buffers */
        for (uint32_t i = 0; i < IDENTIFICATION_CHUNK_SIZE; i++)
        {
            m_h_identification->refMicSum[i + IDENTIFICATION_CHUNK_SIZE]
                = (uint32_t) refMicBfr[i];
            m_h_identification->errMicSum[i + IDENTIFICATION_CHUNK_SIZE]
                = (uint32_t) errMicBfr[i];
        }

        m_h_identification->sumCycles--;
        /* End of identification */
        if (m_h_identification->sumCycles == 0)
        {
            anc_acquisition_stop(m_h_identification->h_ancAcquisition);
            m_h_identification->done = 1;
            identification_onDoneCallback(
                m_h_identification->refMicSum,
                m_h_identification->errMicSum
            );
        }
    }
}
