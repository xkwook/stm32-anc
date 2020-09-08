/*
 * anc_gain.c
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#include "anc_gain.h"

#include "main.h"

anc_gain_t m_gainRefMic;
anc_gain_t m_gainErrMic;
anc_gain_t m_gainOutDac;

static inline void setGain(
    anc_gain_t      gain,
    GPIO_TypeDef**  GPIOx,
    uint32_t*       PinMask
);

void anc_gain_set(anc_gain_channel_t channel, anc_gain_t gain)
{
    static const GPIO_TypeDef* GPIOx_RefMic[2] = {
        REF_A0_GPIO_Port, REF_A1_GPIO_Port
    };
    static const GPIO_TypeDef* GPIOx_ErrMic[2] = {
        ERR_A0_GPIO_Port, ERR_A1_GPIO_Port
    };
    static const GPIO_TypeDef* GPIOx_OutDac[2] = {
        OUT_A0_GPIO_Port, OUT_A1_GPIO_Port
    };
    static const uint32_t PinMask_RefMic[2] = {
        REF_A0_Pin, REF_A1_Pin
    };
    static const uint32_t PinMask_ErrMic[2] = {
        ERR_A0_Pin, ERR_A1_Pin
    };
    static const uint32_t PinMask_OutDac[2] = {
        OUT_A0_Pin, OUT_A1_Pin
    };
    switch (channel)
    {
        case ANC_GAIN_REF_MIC:
            setGain(gain, (GPIO_TypeDef**)GPIOx_RefMic, (uint32_t*)PinMask_RefMic);
            m_gainRefMic = gain;
            break;
        case ANC_GAIN_ERR_MIC:
            setGain(gain, (GPIO_TypeDef**)GPIOx_ErrMic, (uint32_t*)PinMask_ErrMic);
            m_gainErrMic = gain;
            break;
        case ANC_GAIN_OUT_DAC:
            setGain(gain, (GPIO_TypeDef**)GPIOx_OutDac, (uint32_t*)PinMask_OutDac);
            m_gainOutDac = gain;
            break;
        default:
            break;
    }
}

anc_gain_t anc_gain_get(anc_gain_channel_t channel)
{
    switch (channel)
    {
        case ANC_GAIN_REF_MIC:
            return m_gainRefMic;
            break;
        case ANC_GAIN_ERR_MIC:
            return m_gainErrMic;
            break;
        case ANC_GAIN_OUT_DAC:
            return m_gainOutDac;
            break;
        default:
            break;
    }
    return ANC_GAIN_2;
}

static inline void setGain(
    anc_gain_t      gain,
    GPIO_TypeDef**  GPIOx,
    uint32_t*       PinMask
)
{
    switch (gain)
    {
        case ANC_GAIN_2:
            LL_GPIO_ResetOutputPin(GPIOx[0], PinMask[0]);
            LL_GPIO_ResetOutputPin(GPIOx[1], PinMask[1]);
            break;
        case ANC_GAIN_4:
            LL_GPIO_SetOutputPin(GPIOx[0], PinMask[0]);
            LL_GPIO_ResetOutputPin(GPIOx[1], PinMask[1]);
            break;
        case ANC_GAIN_10:
            LL_GPIO_ResetOutputPin(GPIOx[0], PinMask[0]);
            LL_GPIO_SetOutputPin(GPIOx[1], PinMask[1]);
            break;
        case ANC_GAIN_20:
            LL_GPIO_SetOutputPin(GPIOx[0], PinMask[0]);
            LL_GPIO_SetOutputPin(GPIOx[1], PinMask[1]);
            break;
        default:
            break;
    }
}
