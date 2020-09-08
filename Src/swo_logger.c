/*
 * swo_logger.c
 *
 *  Created on: Sep 6, 2020
 *      Author: klukomski
 */

#include "swo_logger.h"

#include "main.h"
#include <stdio.h>
#include <stdarg.h>

static uint32_t m_priMask;

#define CRITICAL_SECTION_BEGIN()  {     \
        m_priMask = __get_PRIMASK();    \
        __disable_irq();                \
    }

#define CRITICAL_SECTION_END()  {       \
        if (!m_priMask)                 \
        {                               \
            __enable_irq();             \
        }                               \
    }

/* Logger object */
swo_logger_t m_swo_logger;

/* Public methods definition */

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

    CRITICAL_SECTION_BEGIN();
    char* dest_p = self->write_p;
    /* Write pointer is clearly overhead of read pointer */
    if (dest_p > self->read_p)
    {
        /* Go with write to beginning if it would exceed bfr */
        if (dest_p + length > self->bfrEnd_p)
        {
            /* Mark next buffer as null string */
            *(dest_p) = '\0';
            /* Skip to beginning of buffer */
            dest_p = self->bfr;
        }
    }

    /* Write pointer can overlap read pointer */
    if (dest_p <= self->read_p)
    {
        /* If this overlapping of read pointer would happen */
        if ((dest_p + length) >= self->read_p)
        {
            /* Return logging error no buffer left */
            va_end(args);
            CRITICAL_SECTION_END();
            return -1;
        }
    }

    /* If there was no message in read pointer, put this as first message */
    if (self->read_p == NULL)
    {
        self->read_p = dest_p;
    }

    /* Move write pointer ahead of message */
    self->write_p = dest_p + length + 1;
    /* Skip to the beginning of buffer if overlaps end */
    if (self->write_p > self->bfrEnd_p)
    {
        self->write_p = self->bfr;
    }
    CRITICAL_SECTION_END();

    /* Write log message to buffer */
    vsprintf(dest_p, format, args);
    va_end(args);

    return length;
}

int swo_logger_process(swo_logger_t* self)
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

    if (self->read_p == self->bfrEnd_p)
    {
        self->read_p = self->bfr;
    }
    else
    {
        (self->read_p)++;
        if (self->read_p != self->write_p)
        {
            if (*(self->read_p) == '\0')
            {
                self->read_p = self->bfr;
            }
        }
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
    while (!swo_logger_process(self));
}
