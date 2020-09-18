/*
 * agc.c
 *
 *  Created on: Sep 18, 2020
 *      Author: klukomski
 */

#include "agc.h"

/* Private defines */

#define GAIN_2              2u
#define GAIN_4              4u
#define GAIN_10             10u
#define GAIN_20             20u

#define MARGIN_MAX          2048u

#define MARGIN_MIN          128u

#define MARGIN_GAIN_2       ((MARGIN_MAX * GAIN_2 / GAIN_4)   + MARGIN_MIN)
#define MARGIN_GAIN_4       ((MARGIN_MAX * GAIN_4 / GAIN_10)  + MARGIN_MIN)
#define MARGIN_GAIN_10      ((MARGIN_MAX * GAIN_10 / GAIN_20) + MARGIN_MIN)


/* Private methods declaration */

static inline void init_channel(agc_channel_t* self);

static inline anc_gain_t update_gain(anc_gain_t gain, uint16_t ampMargin);

/* Public methods definition */

void agc_init(agc_t* self)
{
    self->counter = AGC_HORIZON;

    init_channel(&(self->refChannel));
    init_channel(&(self->errChannel));
    init_channel(&(self->outChannel));
}

inline void agc_adapt(
    agc_t*      self,
    uint16_t*   refData_p,
    uint16_t*   errData_p,
    uint16_t*   outData_p
)
{
    uint16_t ampMarginRef, ampMarginErr, ampMarginOut;
    uint16_t marginRef0, marginErr0, marginOut0;
    uint16_t marginRef1, marginErr1, marginOut1;
    uint32_t i;

    /* Load Amplitude Margin variables */
    ampMarginRef = self->refChannel.ampMargin;
    ampMarginErr = self->errChannel.ampMargin;
    ampMarginOut = self->outChannel.ampMargin;

    for (i = 0; i < AGC_CHUNK_SIZE; i++)
    {
        /* Load samples as margins to zero */
        marginRef0 = *refData_p++;
        marginErr0 = *errData_p++;
        marginOut0 = *outData_p++;

        /* Calculate margins to 1 (4095) - max ADC value */
        marginRef1 = 4095 - marginRef0;
        marginErr1 = 4095 - marginErr0;
        marginOut1 = 4095 - marginOut0;

        /* Check ref margins */
        if (marginRef0 < ampMarginRef)
        {
            ampMarginRef = marginRef0;
        }
        if (marginRef1 < ampMarginRef)
        {
            ampMarginRef = marginRef1;
        }

        /* Check err margins */
        if (marginErr0 < ampMarginErr)
        {
            ampMarginErr = marginErr0;
        }
        if (marginErr1 < ampMarginErr)
        {
            ampMarginErr = marginErr1;
        }

        /* Check out margins */
        if (marginOut0 < ampMarginOut)
        {
            ampMarginOut = marginOut0;
        }
        if (marginOut1 < ampMarginOut)
        {
            ampMarginOut = marginOut1;
        }
    }

    /* Decrement counter and update gains at the end of horizon */
    if (self->counter > 0u)
    {
        /* Decrement horizon counter */
        (self->counter)--;
    }
    else
    {
        anc_gain_t refGain, errGain, outGain;

        refGain = anc_gain_refGet();
        errGain = anc_gain_errGet();
        outGain = anc_gain_outGet();

        /* Update gains */
        anc_gain_refSet(
            update_gain(refGain, ampMarginRef)
        );

        anc_gain_errSet(
            update_gain(errGain, ampMarginErr)
        );

        anc_gain_outSet(
            update_gain(outGain, ampMarginOut)
        );

        /* Reset counter */
        self->counter = AGC_HORIZON;
    }

    /* Save result margins for block */
    self->refChannel.ampMargin = ampMarginRef;
    self->errChannel.ampMargin = ampMarginErr;
    self->outChannel.ampMargin = ampMarginOut;
}

/* Private methods definition */

static inline void init_channel(agc_channel_t* self)
{
    self->ampMargin = MARGIN_MAX;
}

static inline anc_gain_t update_gain(anc_gain_t gain, uint16_t ampMargin)
{
    anc_gain_t retGain = gain;
    switch (gain)
    {
    case ANC_GAIN_2:
        if (ampMargin >= MARGIN_GAIN_2)
        {
            retGain = ANC_GAIN_4;
        }
        break;

    case ANC_GAIN_4:
        if (ampMargin < MARGIN_MIN)
        {
            retGain = ANC_GAIN_2;
        }
        if (ampMargin >= MARGIN_GAIN_4)
        {
            retGain = ANC_GAIN_10;
        }
        break;

    case ANC_GAIN_10:
        if (ampMargin < MARGIN_MIN)
        {
            retGain = ANC_GAIN_4;
        }
        if (ampMargin >= MARGIN_GAIN_10)
        {
            retGain = ANC_GAIN_20;
        }
        break;

    case ANC_GAIN_20:
        if (ampMargin < MARGIN_MIN)
        {
            retGain = ANC_GAIN_10;
        }
        break;
   }

   return retGain;
}