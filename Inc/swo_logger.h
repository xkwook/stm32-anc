/*
 * swo_logger.h
 *
 *  Created on: Sep 6, 2020
 *      Author: klukomski
 */

#ifndef SWO_LOGGER_H_
#define SWO_LOGGER_H_

#include "main.h"
#include <stdio.h>
#include <stdarg.h>

#define SWO_LOGGER_BFR_LENGTH       1024

#define SWO_LOG_INFO

struct swo_logger_struct {
    char*   read_p;
    char*   write_p;
    char*   bfrEnd_p;
    char    bfr[SWO_LOGGER_BFR_LENGTH];
};

typedef struct swo_logger_struct swo_logger_t;

void swo_logger_init(swo_logger_t* self)
{
    self->write_p   = self->bfr;
    self->read_p    = NULL;
    self->bfrEnd_p  = self->bfr + SWO_LOGGER_BFR_LENGTH -1;
}

int swo_logger_log(swo_logger_t* self, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int length = vsnprintf(NULL, 0, format, args);
    /* Write pointer is clearly overhead of read pointer */
    if (self->write_p > self->read_p)
    {
        /* Go with write to beginning if it would exceed bfr */
        if (self->write_p + length > self->bfrEnd_p)
        {
            /* Mark next buffer as null string */
            *(self->write_p) = '\0';
            /* Skip to beginning of buffer */
            self->write_p = self->bfr;
        }
    }
    /* Write pointer can overlap read pointer */
    if (self->write_p <= self->read_p)
    {
        /* If this overlapping of read pointer would happen */
        if ((self->write_p + length) >= self->read_p)
        {
            /* Return logging error no buffer left */
            va_end(args);
            return -1;
        }
    }

    /* Write log message to buffer */
    vsprintf(self->write_p, format, args);
    va_end(args);

    /* If there was no message in read pointer, put this as first message */
    if (self->read_p == NULL)
    {
        self->read_p = self->write_p;
    }

    /* Move write pointer ahead of message */
    self->write_p = self->write_p + length + 1;
    /* Skip to the beginning of buffer if overlaps end */
    if (self->write_p > self->bfrEnd_p)
    {
        self->write_p = self->bfr;
    }

    return length;
}

int swo_logger_flush_one(swo_logger_t* self)
{
    const char cr = '\r';
    const char nl = '\n';
    if (self->read_p == NULL)
    {
        /* Error no buffer to read */
        return -1;
    }

    while (*(self->read_p) != '\0')
    {
        ITM_SendChar(*(self->read_p));
        (self->read_p)++;
    }
    /* Send \r\n chars */
    ITM_SendChar(cr);
    ITM_SendChar(nl);

    (self->read_p)++;
    if (self->read_p > self->bfrEnd_p)
    {
        self->read_p = self->bfr;
    }
    else if (*(self->read_p) == '\0')
    {
        self->read_p = self->bfr
    }

    if (self->read_p == self->write_p)
    {
        self->read_p = NULL;
    }

    /* Return success */
    return 0;
}

void swo_logger_flush(swo_logger_t* self)
{
    while (swo_logger_flush_one(self));
}

#endif /* SWO_LOGGER_H_ */