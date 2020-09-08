/*
 * anc_gain.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_GAIN_H_
#define ANC_GAIN_H_

/* Gains 2, 4, 10, 20 */

typedef enum
{
    ANC_GAIN_REF_MIC,
    ANC_GAIN_ERR_MIC,
    ANC_GAIN_OUT_DAC
} anc_gain_channel_t;

typedef enum
{
    ANC_GAIN_2,
    ANC_GAIN_4,
    ANC_GAIN_10,
    ANC_GAIN_20
} anc_gain_t;

void anc_gain_set(anc_gain_channel_t channel, anc_gain_t gain);

anc_gain_t anc_gain_get(anc_gain_channel_t channel);

#endif /* ANC_GAIN_H_ */