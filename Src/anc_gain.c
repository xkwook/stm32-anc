/*
 * anc_gain.c
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#include "anc_gain.h"

#include "main.h"

/* Private variables */

static anc_gain_t m_gainRefMic;
static anc_gain_t m_gainErrMic;
static anc_gain_t m_gainOutDac;

/* Private methods declaration */

static inline void setGain(
    anc_gain_t      gain,
    GPIO_TypeDef*   GPIOx_A0,
    GPIO_TypeDef*   GPIOx_A1,
    uint32_t        PinMask_A0,
    uint32_t        PinMask_A1
);

/* Public methods definition */

inline void anc_gain_refSet(anc_gain_t gain)
{
    setGain(gain,
        REF_A0_GPIO_Port, REF_A1_GPIO_Port,
        REF_A0_Pin, REF_A1_Pin
    );
}

inline void anc_gain_errSet(anc_gain_t gain)
{
    setGain(gain,
        ERR_A0_GPIO_Port, ERR_A1_GPIO_Port,
        ERR_A0_Pin, ERR_A1_Pin
    );
}

inline void anc_gain_outSet(anc_gain_t gain)
{
    setGain(gain,
        OUT_A0_GPIO_Port, OUT_A1_GPIO_Port,
        OUT_A0_Pin, OUT_A1_Pin
    );
}

inline anc_gain_t anc_gain_refGet(void)
{
    return m_gainRefMic;
}

inline anc_gain_t anc_gain_errGet(void)
{
    return m_gainErrMic;
}

inline anc_gain_t anc_gain_outGet(void)
{
    return m_gainOutDac;
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
