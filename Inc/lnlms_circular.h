/*
 * lnlms_circular.h
 *
 *  Created on: Sep 17, 2020
 *      Author: klukomski
 */

#ifndef LNLMS_CIRCULAR_H_
#define LNLMS_CIRCULAR_H_

#include "anc_math.h"

struct lnlms_circular_struct {
    q15_t*      coeffs_p;
    q15_t       alpha;
    float       mu_f;
    q15_t*      stateBfr_p;
    uint32_t    length;
    q15_t       energy;
};

typedef struct lnlms_circular_struct lnlms_circular_t;

/* Public methods declaration */

void lnlms_circular_init(
    lnlms_circular_t*   self,
    q15_t*              coeffs_p,
    q15_t               alpha,
    float               mu_f,
    uint32_t            length
);

inline void lnlms_circular_update(
    lnlms_circular_t*   self,
    q15_t               error
);

#endif /* LNLMS_CIRCULAR_H_ */
