/*
 * anc_cmd.h
 *
 *  Created on: Sep 8, 2020
 *      Author: klukomski
 */

#ifndef ANC_CMD_H_
#define ANC_CMD_H_

#include <string.h>

typedef enum
{
    ANC_CMD_WRONG_CMD,
    ANC_CMD_START,
    ANC_CMD_STOP
} anc_cmd_t;

#define ANC_CMD_START_STR       "start"
#define ANC_CMD_STOP_STR        "stop"

anc_cmd_t anc_cmd_decode(const char* const cmd)
{
    if (strncmp(cmd, ANC_CMD_START_STR, strlen(ANC_CMD_START_STR)) == 0)
    {
        return ANC_CMD_START;
    }

    if (strncmp(cmd, ANC_CMD_STOP_STR, strlen(ANC_CMD_STOP_STR)) == 0)
    {
        return ANC_CMD_STOP;
    }

    return ANC_CMD_WRONG_CMD;
}

#endif /* ANC_CMD_H_ */