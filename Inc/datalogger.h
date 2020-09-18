/*
 * datalogger.h
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#ifndef DATALOGGER_H_
#define DATALOGGER_H_

#include "main.h"

#define DATALOGGER_SAMPLE_TYPE      q15_t
#define DATALOGGER_SAMPLE_CHANNELS  3
#define DATALOGGER_SAMPLE_BURST     1

/* Logging data structure */

struct DataLogger_logData_struct {
    DATALOGGER_SAMPLE_TYPE  sample[DATALOGGER_SAMPLE_CHANNELS][DATALOGGER_SAMPLE_BURST];
};

typedef struct __attribute__((packed)) DataLogger_logData_struct
    DataLogger_logData_t;

/* DataLogger class body */

struct DataLogger_struct {
    DataLogger_logData_t    bfr;
};

typedef struct DataLogger_struct
    DataLogger_t;

/* Public methods declaration */

void DataLogger_Init(DataLogger_t* self);

int  DataLogger_Log(DataLogger_t* self);

void DataLogger_DmaIrqHandler(DataLogger_t* self);

__attribute__((weak)) void DataLogger_TransferCpltCallback(DataLogger_t* self);

#endif /* DATALOGGER_H_ */
