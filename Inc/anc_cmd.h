/*
 * anc_cmd.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_CMD_H_
#define ANC_CMD_H_

#include <stdint.h>

/* Available commands:
 *  - start
 *  - stop
 *  - anc-on
 *  - anc-off
 *  - agc-on
 *  - agc-off
 *  - set-gains=x,y,z
 *  - set-gains ref=x err=y out=z   # maybe this version?
 * These work only in stop mode:
 *  - help
 *  - identification
 *  - identification-get=ref
 *  - identification-get=err
 *  - offline-identification
 *  - set-offline-lms-mu=x
 *  - set-anc-lms-mu=x
 */

typedef enum
{
    ANC_CMD_WRONG_CMD,
    ANC_CMD_START,
    ANC_CMD_STOP,
    ANC_CMD_ANC_ON,
    ANC_CMD_ANC_OFF,
    ANC_CMD_AGC_ON,
    ANC_CMD_AGC_OFF,
    ANC_CMD_SET_GAINS,
    ANC_CMD_HELP,
    ANC_CMD_IDENTIFICATION_GET_REF,
    ANC_CMD_IDENTIFICATION_GET_ERR,
    ANC_CMD_IDENTIFICATION,
    ANC_CMD_OFFLINE_IDENTIFICATION,
    ANC_CMD_SET_OFFLINE_LMS_PARAMS,
    ANC_CMD_SET_ANC_LMS_PARAMS,
    ANC_CMD_PERFORMANCE,
    ANC_CMD_COEFFS
} anc_cmd_t;

anc_cmd_t anc_cmd_decode(volatile char* cmd, uint8_t** retCmdData);

#endif /* ANC_CMD_H_ */
