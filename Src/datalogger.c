/*
 * datalogger.c
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#include "datalogger.h"

#define LOGGER_IDLE         0
#define LOGGER_RUNNING      1

/* Public methods definition */

void DataLogger_Init(DataLogger_t* self)
{
    /* Enable DMA mode in UART */
    LL_USART_EnableDMAReq_TX(DATALOGGER_USART);

    /* According to reference manual DMA stream configuration procedure */
    LL_DMA_SetPeriphAddress(DATALOGGER_DMA, DATALOGGER_DMA_STREAM,
        LL_USART_DMA_GetRegAddr(DATALOGGER_USART)
    );
    LL_DMA_SetMemoryAddress(DATALOGGER_DMA, DATALOGGER_DMA_STREAM,
        (uint32_t)&self->bfr
    );
    LL_DMA_SetDataLength(DATALOGGER_DMA, DATALOGGER_DMA_STREAM,
        sizeof(self->bfr)
    );

    /* Enable Transfer Completed interrupt */
    LL_DMA_EnableIT_TC(DATALOGGER_DMA, DATALOGGER_DMA_STREAM);
}

int  DataLogger_Log(DataLogger_t* self)
{
    if (LL_DMA_IsEnabledStream(DATALOGGER_DMA, DATALOGGER_DMA_STREAM))
    {
        /* Return error */
        return -1;  // need to do error
    }

    LL_DMA_EnableStream(DATALOGGER_DMA, DATALOGGER_DMA_STREAM);

    /* Return success */
    return 0;
}

void DataLogger_DmaIrqHandler(DataLogger_t* self)
{
    int transferCompletedFlag;
    #if (DATALOGGER_DMA_STREAM == LL_DMA_STREAM_7)
        transferCompletedFlag = LL_DMA_IsActiveFlag_TC7(DATALOGGER_DMA);
    #endif
    if (transferCompletedFlag)
    {
        #if (DATALOGGER_DMA_STREAM == LL_DMA_STREAM_7)
            LL_DMA_ClearFlag_TC7(DATALOGGER_DMA);
        #endif
        DataLogger_TransferCpltCallback(self);
    }
}

void DataLogger_TransferCpltCallback(DataLogger_t* self)
{
    static int counter;
    for (int i = 0; i < DATALOGGER_SAMPLE_CHANNELS; i++)
    {
        for (int j = 0; j < DATALOGGER_SAMPLE_BURST; j++)
        {
            counter++;
            self->bfr.sample[i][j] = (DATALOGGER_SAMPLE_TYPE)((i + j) * counter);
        }
    }
    DataLogger_Log(self);
}