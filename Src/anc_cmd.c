/*
 * anc_cmd.c
 *
 *  Created on: Sep 12, 2020
 *      Author: klukomski
 */

#include "anc_cmd.h"
#include "anc_gain.h"

#include <string.h>
#include <stdlib.h>

#define START_STR                   "start"
#define STOP_STR                    "stop"
#define ANC_ON_STR                  "anc-on"
#define ANC_OFF_STR                 "anc-off"
#define AGC_ON_STR                  "agc-on"
#define AGC_OFF_STR                 "agc-off"
#define SET_GAINS_STR               "set-gains"

#define HELP_STR                    "help"
#define IDENTIFICATION_GET_REF_STR  "identification-get=ref"
#define IDENTIFICATION_GET_ERR_STR  "identification-get=err"
#define IDENTIFICATION_STR          "identification"
#define OFFLINE_IDENTIFICATION_STR  "offline-identification"
#define SET_OFFLINE_LMS_MI_STR      "set-offline-lms-mu"
#define SET_ANC_LMS_MI_STR          "set-anc-lms-mu"
#define PERFORMANCE_STR             "performance"

#define CMD_BFR_MAXLEN              128

//#define HELP_CONTENT_STR            ""

#define ADD_CMD(str, retEnum)                       \
    do {                                            \
        if (strncmp(cmdBfr, str, strlen(str)) == 0) \
        {                                           \
            return retEnum;                         \
        }                                           \
    } while(0);                                     \


static int decodeIntegers(const char* cmd, int32_t* decodedData, const uint32_t maxNum);

anc_cmd_t anc_cmd_decode(volatile char* cmd, uint8_t** retCmdData)
{
    static uint8_t cmdData[128];
    char cmdBfr[CMD_BFR_MAXLEN];

    /* Copy command string to internal buffer */
    for (int i = 0; i < CMD_BFR_MAXLEN; i++)
    {
        cmdBfr[i] = (char) cmd[i];
        if (cmd[i] == '\0')
        {
            break;
        }
    }

    *retCmdData = NULL;

    ADD_CMD(START_STR,
            ANC_CMD_START);
    ADD_CMD(STOP_STR,
            ANC_CMD_STOP);
    ADD_CMD(ANC_ON_STR,
            ANC_CMD_ANC_ON);
    ADD_CMD(ANC_OFF_STR,
            ANC_CMD_ANC_OFF);
    ADD_CMD(AGC_ON_STR,
            ANC_CMD_AGC_ON);
    ADD_CMD(AGC_OFF_STR,
            ANC_CMD_AGC_OFF);
    /* Set gains cmd */
    if (strncmp(cmdBfr, SET_GAINS_STR, strlen(SET_GAINS_STR)) == 0)
    {
        uint32_t num = decodeIntegers(cmdBfr, (int32_t*)cmdData, 128 / sizeof(int32_t));

        if (num < 3)
        {
            return ANC_CMD_WRONG_CMD;
        }

        *retCmdData = cmdData;

        return ANC_CMD_SET_GAINS;
    }

    ADD_CMD(HELP_STR,
            ANC_CMD_HELP);
    ADD_CMD(IDENTIFICATION_GET_REF_STR,
            ANC_CMD_IDENTIFICATION_GET_REF);
    ADD_CMD(IDENTIFICATION_GET_ERR_STR,
            ANC_CMD_IDENTIFICATION_GET_ERR);
    ADD_CMD(IDENTIFICATION_STR,
            ANC_CMD_IDENTIFICATION);
    ADD_CMD(OFFLINE_IDENTIFICATION_STR,
            ANC_CMD_OFFLINE_IDENTIFICATION);
    ADD_CMD(PERFORMANCE_STR,
            ANC_CMD_PERFORMANCE);
    /* Set mi commands */

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
