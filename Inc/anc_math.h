/*
 * anc_math.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef ANC_MATH_H_
#define ANC_MATH_H_

#include <stdint.h>
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
#else /* Normal implementation for Cortex-M4 processor */
#include "cmsis_armcc.h"
#endif /* end of ANC_MATH_TESTING_IMPLEMENTATION */

typedef int16_t q15_t;

typedef int32_t q31_t;

typedef int64_t q63_t;

  /*
   * @brief C custom defined intrinisic function for testing on PC
   */
#if defined(ANC_MATH_TESTING_IMPLEMENTATION)
  static __INLINE q31_t __SSAT(
  q31_t x,
  uint32_t y)
  {
    int32_t posMax, negMin;
    uint32_t i;

    posMax = 1;
    for (i = 0; i < (y - 1); i++)
    {
      posMax = posMax * 2;
    }

    if(x > 0)
    {
      posMax = (posMax - 1);

      if(x > posMax)
      {
        x = posMax;
      }
    }
    else
    {
      negMin = -posMax;

      if(x < negMin)
      {
        x = negMin;
      }
    }
    return (x);
  }
#endif /* end of ANC_MATH_TESTING_IMPLEMENTATION */

#endif /* ANC_MATH_H_ */