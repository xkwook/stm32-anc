/*
 * swo_logger.h
 *
 *  Created on: Sep 6, 2020
 *      Author: klukomski
 */

#ifndef SWO_LOGGER_H_
#define SWO_LOGGER_H_

#define SWO_LOGGER_BFR_LENGTH       2048

struct swo_logger_struct {
    char*   read_p;
    char*   write_p;
    char*   bfrEnd_p;
    char    bfr[SWO_LOGGER_BFR_LENGTH];
};

typedef struct swo_logger_struct swo_logger_t;

extern swo_logger_t m_swo_logger;

/* Useful macros for easier tasks using logger instance */

#define SWO_LOG_INIT()              swo_logger_init(&m_swo_logger)
#define SWO_LOG(...)                swo_logger_log(&m_swo_logger, __VA_ARGS__)
#define SWO_LOG_PROCESS()           swo_logger_process(&m_swo_logger)
#define SWO_LOG_FLUSH()             swo_logger_flush(&m_swo_logger)

void swo_logger_init(swo_logger_t* self);

int swo_logger_log(swo_logger_t* self, const char* format, ...);

int swo_logger_process(swo_logger_t* self);

void swo_logger_flush(swo_logger_t* self);

#endif /* SWO_LOGGER_H_ */
