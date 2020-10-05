/*
 * anc_gain.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_GAIN_H_
#define ANC_GAIN_H_

#include "main.h"

/* Gains 2, 4, 10, 20 */

typedef enum
{
    ANC_GAIN_2  = 0,
    ANC_GAIN_4  = 1,
    ANC_GAIN_10 = 2,
    ANC_GAIN_20 = 3
} anc_gain_t;

typedef struct
{
    anc_gain_t gainRefMic;
    anc_gain_t gainErrMic;
    anc_gain_t gainOutDac;
} anc_gainPrvData_t;

anc_gainPrvData_t   anc_gainPrvData;

/* Private methods declaration */

static inline void setGain(
    anc_gain_t      gain,
    GPIO_TypeDef*   GPIOx_A0,
    GPIO_TypeDef*   GPIOx_A1,
    uint32_t        PinMask_A0,
    uint32_t        PinMask_A1
);

/* Public inline methods definition */

static inline void anc_gain_refSet(anc_gain_t gain)
{
    anc_gainPrvData.gainRefMic = gain;
    setGain(gain,
        REF_A0_GPIO_Port, REF_A1_GPIO_Port,
        REF_A0_Pin, REF_A1_Pin
    );
}

static inline void anc_gain_errSet(anc_gain_t gain)
{
    anc_gainPrvData.gainErrMic = gain;
    setGain(gain,
        ERR_A0_GPIO_Port, ERR_A1_GPIO_Port,
        ERR_A0_Pin, ERR_A1_Pin
    );
}

static inline void anc_gain_outSet(anc_gain_t gain)
{
    anc_gainPrvData.gainOutDac = gain;
    setGain(gain,
        OUT_A0_GPIO_Port, OUT_A1_GPIO_Port,
        OUT_A0_Pin, OUT_A1_Pin
    );
}

static inline anc_gain_t anc_gain_refGet(void)
{
    return anc_gainPrvData.gainRefMic;
}

static inline anc_gain_t anc_gain_errGet(void)
{
    return anc_gainPrvData.gainErrMic;
}

static inline anc_gain_t anc_gain_outGet(void)
{
    return anc_gainPrvData.gainOutDac;
}

/* Private methods definition */

static inline void setGain(
    anc_gain_t      gain,
    GPIO_TypeDef*   GPIOx_A0,
    GPIO_TypeDef*   GPIOx_A1,
    uint32_t        PinMask_A0,
    uint32_t        PinMask_A1
)
{
    switch (gain)
    {
        case ANC_GAIN_2:
            LL_GPIO_ResetOutputPin(GPIOx_A0, PinMask_A0);
            LL_GPIO_ResetOutputPin(GPIOx_A0, PinMask_A1);
            break;
        case ANC_GAIN_4:
            LL_GPIO_SetOutputPin(GPIOx_A0, PinMask_A0);
            LL_GPIO_ResetOutputPin(GPIOx_A1, PinMask_A1);
            break;
        case ANC_GAIN_10:
            LL_GPIO_ResetOutputPin(GPIOx_A0, PinMask_A0);
            LL_GPIO_SetOutputPin(GPIOx_A1, PinMask_A1);
            break;
        case ANC_GAIN_20:
            LL_GPIO_SetOutputPin(GPIOx_A0, PinMask_A0);
            LL_GPIO_SetOutputPin(GPIOx_A1, PinMask_A1);
            break;
    }
}

#endif /* ANC_GAIN_H_ */