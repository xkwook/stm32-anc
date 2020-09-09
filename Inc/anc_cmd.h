/*
 * anc_cmd.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_CMD_H_
#define ANC_CMD_H_

#include "anc_gain.h"

#include <string.h>
#include <stdlib.h>

typedef enum
{
    ANC_CMD_WRONG_CMD,
    ANC_CMD_START,
    ANC_CMD_STOP,
    ANC_CMD_SET_GAINS,
    ANC_CMD_IDENTIFICATION,
    ANC_CMD_IDENTIFICATION_RESULTS,
    ANC_CMD_OFFLINE_IDENTIFICATION
} anc_cmd_t;

#define ANC_CMD_START_STR                   "start"
#define ANC_CMD_STOP_STR                    "stop"
#define ANC_CMD_SET_GAINS_STR               "set-gains"
#define ANC_CMD_IDENTIFICATION_STR          "identification"
#define ANC_CMD_IDENTIFICATION_RESULTS_STR  "identification-results"
#define ANC_CMD_OFFLINE_IDENTIFICATION_STR  "offline-identification"

static int decodeIntegers(const char* cmd, int32_t* decodedData, const uint32_t maxNum);

anc_cmd_t anc_cmd_decode(const char* const cmd, uint8_t** retCmdData)
{
    static uint8_t cmdData[128];

    *retCmdData = NULL;

    if (strncmp(cmd, ANC_CMD_START_STR, strlen(ANC_CMD_START_STR)) == 0)
    {
        return ANC_CMD_START;
    }

    if (strncmp(cmd, ANC_CMD_STOP_STR, strlen(ANC_CMD_STOP_STR)) == 0)
    {
        return ANC_CMD_STOP;
    }

    if (strncmp(cmd, ANC_CMD_SET_GAINS_STR, strlen(ANC_CMD_SET_GAINS_STR)) == 0)
    {
        uint32_t num = decodeIntegers(cmd, cmdData, 128 / sizeof(int32_t));

        *retCmdData = cmdData;

        return ANC_CMD_SET_GAINS;
    }

    if (strncmp(cmd, ANC_CMD_IDENTIFICATION_STR, strlen(ANC_CMD_IDENTIFICATION_STR)) == 0)
    {
        return ANC_CMD_IDENTIFICATION;
    }

    return ANC_CMD_WRONG_CMD;
}

static int decodeIntegers(const char* cmd, int32_t* decodedData, const uint32_t maxNum)
{
    const char delim[2] = ",";
    char* token;
    int num = 0;

    /* Find = character and move to comma separated integers */
    char* data_p = strchr(cmd, '=');
    data_p++;

    /* get the first token */
    token = strtok(data_p, delim);

    /* walk through other tokens */
    while( token != NULL ) {
        decodedData[num]
            = (int32_t)strtol(token, (char**)NULL, 10);
        num++;
        if (num == maxNum)
        {
            break;
        }
        token = strtok(NULL, delim);
    }

    return num;
}

#endif /* ANC_CMD_H_ */
