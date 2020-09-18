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
    ANC_GAIN_2  = 0,
    ANC_GAIN_4  = 1,
    ANC_GAIN_10 = 2,
    ANC_GAIN_20 = 3
} anc_gain_t;

inline void anc_gain_refSet(anc_gain_t gain);

inline void anc_gain_errSet(anc_gain_t gain);

inline void anc_gain_outSet(anc_gain_t gain);

inline anc_gain_t anc_gain_refGet(void);

inline anc_gain_t anc_gain_errGet(void);

inline anc_gain_t anc_gain_outGet(void);

#endif /* ANC_GAIN_H_ */