/*
 * anc_application.c
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#include "anc_application.h"

#include "anc_parameters.h"
#include "anc_math.h"
#include "anc_cmd.h"
#include "anc_gain.h"
#include "identification.h"
#include "agc.h"
#include "anc_processing.h"
#include "anc_algorithm.h"
#include "anc_offline_identification.h"
#include "swo_logger.h"
#include "performance.h"

typedef enum
{
    ANC_APPLICATION_IDENTIFICATION_EMPTY,
    ANC_APPLICATION_IDENTIFICATION_READY
} anc_application_identificationState_t;

struct anc_application_struct
{
    /* Object handlers */
    anc_acquisition_t*  h_ancAcquisition;
    uart_receiver_t*    h_uartReceiver;
    uart_transmitter_t* h_uartTransmitter;
    dma_mem2mem_t*      h_dmaMem2mem0;
    dma_mem2mem_t*      h_dmaMem2mem1;
    /* Private objects */
    identification_t                identification;
    agc_t                           agc;
    anc_processing_logData_t        ancProcessingLogData;
    anc_processing_t                ancProcessing[2];
    anc_algorithm_t                 ancAlgorithm[2];
    performance_t                   performance[2];
    anc_offline_identification_t    ancOfflineIdentification[2];
    /* Private variables */
    anc_application_state_t                 state;
    anc_application_identificationState_t   identificationState;
};

typedef struct anc_application_struct anc_application_t;

static anc_application_t m_app;

/* Private methods declaration */

static inline void IdleStateHandle(anc_application_t* self);
static inline void AcquisitionStateHandle(anc_application_t* self);
static inline void IdentificationStateHandle(anc_application_t* self);
static inline void OfflineIdentificationStateHandle(anc_application_t* self);

void SetGains(uint8_t* cmdData);

/* Callbacks for anc_acquisition module */

void acquisition_bfr0_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
);

void acquisition_bfr1_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
);

void offline_identification_bfr0_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
);

void offline_identification_bfr1_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
);

/* Public methods definition */

void anc_application_init(
    anc_acquisition_t*  h_ancAcquisition,
    uart_receiver_t*    h_uartReceiver,
    uart_transmitter_t* h_uartTransmitter,
    dma_mem2mem_t*      h_dmaMem2mem0,
    dma_mem2mem_t*      h_dmaMem2mem1
)
{
    m_app.h_ancAcquisition  = h_ancAcquisition;
    m_app.h_uartReceiver    = h_uartReceiver;
    m_app.h_uartTransmitter = h_uartTransmitter;
    m_app.h_dmaMem2mem0     = h_dmaMem2mem0;
    m_app.h_dmaMem2mem1     = h_dmaMem2mem1;

    /* Initialize to IDLE (stopped) state */
    m_app.state = ANC_APPLICATION_IDLE;
    m_app.identificationState = ANC_APPLICATION_IDENTIFICATION_EMPTY;

    /* Set all gains to default */
    anc_gain_refSet(ANC_GAIN_2);
    anc_gain_errSet(ANC_GAIN_2);
    anc_gain_outSet(ANC_GAIN_2);

    /* Init objects */
    identification_init(
        &m_app.identification,
        m_app.h_ancAcquisition,
        anc_excitationSignal
    );

    /* Init weigths for LNLMS algorithms */
    lnlms_circular_initCoeffs(
        anc_Sn_coeffs,
        ANC_SN_FILTER_LENGTH
    );

    lnlms_circular_initCoeffs(
        anc_Wn_coeffs,
        ANC_WN_FILTER_LENGTH
    );

    /* Test DMA mem2mem */

    if (dma_mem2mem_test(h_dmaMem2mem0) == DMA_MEM2MEM_SUCCESS)
    {
        SWO_LOG("DmaMem2Mem0 test passed!");
    }
    else
    {
        SWO_LOG("DmaMem2Mem0 test failed!");
    }

    if (dma_mem2mem_test(h_dmaMem2mem1) == DMA_MEM2MEM_SUCCESS)
    {
        SWO_LOG("DmaMem2Mem1 test passed!");
    }
    else
    {
        SWO_LOG("DmaMem2Mem1 test failed!");
    }
}

void anc_application_start(void)
{
    SWO_LOG("ANC application started!");
    uart_receiver_start(m_app.h_uartReceiver);

    /* Infinite loop */
    for (;;)
    {
        switch (m_app.state)
        {
        case ANC_APPLICATION_IDLE:
            IdleStateHandle(&m_app);
            break;

        case ANC_APPLICATION_ACQUISITION:
            AcquisitionStateHandle(&m_app);
            break;

        case ANC_APPLICATION_IDENTIFICATION:
            IdentificationStateHandle(&m_app);
            break;

        case ANC_APPLICATION_OFFLINE_IDENTIFICATION:
            OfflineIdentificationStateHandle(&m_app);
            break;

        default:
            break;
        }
        /* Process logs in the mean time */
        SWO_LOG_PROCESS();
    }
}

anc_application_state_t anc_application_getState(void)
{
    return m_app.state;
}

/* Private methods definition */

static inline void IdleStateHandle(anc_application_t* self)
{
    char* rcvMsg_p;
    uint32_t* bfr;
    uint32_t stabilizingCycles = 16;
    uint32_t sumCycles = 128;
    uint32_t mDelay = 10000;
    rcvMsg_p = uart_receiver_getMsg(self->h_uartReceiver);
    if (rcvMsg_p != UART_RECEIVER_NO_MSG)
    {
        uint8_t* cmdData;
        anc_cmd_t cmd = anc_cmd_decode(rcvMsg_p, &cmdData);
        switch (cmd)
        {
        case ANC_CMD_HELP:
            break;
        case ANC_CMD_START:
            /* Configure acquisition */
            anc_acquisition_configure(
                self->h_ancAcquisition,
                ANC_CHUNK_SIZE,
                acquisition_bfr0_callback,
                acquisition_bfr1_callback
            );

            /* Init algorithm objects */
            agc_init(&self->agc);
            anc_processing_init(
                &self->ancProcessing[0],
                &self->ancProcessing[1],
                &self->agc,
                self->h_uartTransmitter,
                &self->ancProcessingLogData
            );
            anc_algorithm_init(
                &self->ancAlgorithm[0],
                &self->ancAlgorithm[1],
                self->h_dmaMem2mem0,
                self->h_dmaMem2mem1
            );

            anc_acquisition_start(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_ACQUISITION;
            break;
        case ANC_CMD_STOP:
            SWO_LOG("Already stopped!");
            break;
        case ANC_CMD_SET_GAINS:
            SetGains(cmdData);
            break;

        /* Only in Idle state commands */
        case ANC_CMD_IDENTIFICATION:
            identification_configure(&self->identification,
                stabilizingCycles, sumCycles);
            LL_mDelay(mDelay);
            identification_start(&self->identification);
            self->state = ANC_APPLICATION_IDENTIFICATION;
            break;
        case ANC_CMD_IDENTIFICATION_GET_REF:
            bfr = self->identification.refMicSum;
            if (self->identificationState == ANC_APPLICATION_IDENTIFICATION_READY)
            {
                uart_transmitter_setMsg(self->h_uartTransmitter,
                    (uint8_t*)bfr, IDENTIFICATION_BFR_LENGTH * sizeof(bfr[0]));
                uart_transmitter_start(self->h_uartTransmitter);
            }
            break;
        case ANC_CMD_IDENTIFICATION_GET_ERR:
            bfr = self->identification.errMicSum;
            if (self->identificationState == ANC_APPLICATION_IDENTIFICATION_READY)
            {
                uart_transmitter_setMsg(self->h_uartTransmitter,
                    (uint8_t*)bfr, IDENTIFICATION_BFR_LENGTH * sizeof(bfr[0]));
                uart_transmitter_start(self->h_uartTransmitter);
            }
            break;
        case ANC_CMD_OFFLINE_IDENTIFICATION:
            /* Configure acquisition */
            anc_acquisition_configure(
                self->h_ancAcquisition,
                ANC_CHUNK_SIZE,
                offline_identification_bfr0_callback,
                offline_identification_bfr1_callback
            );

            /* Init algorithm objects */
            agc_init(&self->agc);
            anc_processing_init(
                &self->ancProcessing[0],
                &self->ancProcessing[1],
                &self->agc,
                self->h_uartTransmitter,
                &self->ancProcessingLogData
            );
            anc_offline_identification_init(
                &self->ancOfflineIdentification[0],
                &self->ancOfflineIdentification[1],
                self->h_dmaMem2mem0,
                self->h_dmaMem2mem1,
                ANC_OFFLINE_IDENTIFICATION_CYCLES
            );

            //LL_mDelay(mDelay);
            anc_acquisition_start(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_ACQUISITION;
            break;
        case ANC_CMD_SET_OFFLINE_LMS_MI:
            break;
        case ANC_CMD_SET_ANC_LMS_MI:
            break;

        default:
            SWO_LOG("Unsupported command.");
            break;
        }
        SWO_LOG("%s", rcvMsg_p);
        uart_receiver_freeMsg(self->h_uartReceiver);
    }
}

static inline void AcquisitionStateHandle(anc_application_t* self)
{
    char* rcvMsg_p;
    rcvMsg_p = uart_receiver_getMsg(self->h_uartReceiver);
    if (rcvMsg_p != UART_RECEIVER_NO_MSG)
    {
        uint8_t* cmdData;
        anc_cmd_t cmd = anc_cmd_decode(rcvMsg_p, &cmdData);
        switch (cmd)
        {
        case ANC_CMD_START:
            SWO_LOG("Already started!");
            break;
        case ANC_CMD_STOP:
            anc_acquisition_stop(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_IDLE;
            break;
        case ANC_CMD_ANC_ON:
            anc_algorithm_enable(&m_app.ancAlgorithm[0]);
            anc_algorithm_enable(&m_app.ancAlgorithm[1]);
            break;
        case ANC_CMD_ANC_OFF:
            anc_algorithm_disable(&m_app.ancAlgorithm[0]);
            anc_algorithm_disable(&m_app.ancAlgorithm[1]);
            break;
        case ANC_CMD_AGC_ON:
            agc_enable(&m_app.agc);
            break;
        case ANC_CMD_AGC_OFF:
            agc_disable(&m_app.agc);
            break;
        case ANC_CMD_SET_GAINS:
            SetGains(cmdData);
            break;

        case ANC_CMD_PERFORMANCE:
            SWO_LOG("Performance: %5u, %5u",
                performance_get_result(&self->performance[0]),
                performance_get_result(&self->performance[1])
            );
            break;

        default:
            SWO_LOG("Unsupported command.");
            break;
        }
        SWO_LOG("%s", rcvMsg_p);
        uart_receiver_freeMsg(self->h_uartReceiver);
    }

}

static inline void IdentificationStateHandle(anc_application_t* self)
{
    char* rcvMsg_p;
    rcvMsg_p = uart_receiver_getMsg(self->h_uartReceiver);
    if (rcvMsg_p != UART_RECEIVER_NO_MSG)
    {
        uint8_t* cmdData;
        anc_cmd_t cmd = anc_cmd_decode(rcvMsg_p, &cmdData);
        switch (cmd)
        {
        case ANC_CMD_STOP:
            anc_acquisition_stop(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_IDLE;
            break;

        default:
            SWO_LOG("Unsupported command.");
            break;
        }
        SWO_LOG("%s", rcvMsg_p);
        uart_receiver_freeMsg(self->h_uartReceiver);
    }
    if (identification_isDone(&self->identification))
    {
        self->identificationState = ANC_APPLICATION_IDENTIFICATION_READY;
        self->state = ANC_APPLICATION_IDLE;
    }
}

static inline void OfflineIdentificationStateHandle(anc_application_t* self)
{
    char* rcvMsg_p;
    rcvMsg_p = uart_receiver_getMsg(self->h_uartReceiver);
    if (rcvMsg_p != UART_RECEIVER_NO_MSG)
    {
        uint8_t* cmdData;
        anc_cmd_t cmd = anc_cmd_decode(rcvMsg_p, &cmdData);
        switch (cmd)
        {
        case ANC_CMD_STOP:
            anc_acquisition_stop(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_IDLE;
            break;
        case ANC_CMD_SET_GAINS:
            SetGains(cmdData);
            break;

        case ANC_CMD_PERFORMANCE:
            SWO_LOG("Performance: %5u, %5u",
                performance_get_result(&self->performance[0]),
                performance_get_result(&self->performance[1])
            );
            break;

        default:
            SWO_LOG("Unsupported command.");
            break;
        }
        SWO_LOG("%s", rcvMsg_p);
        uart_receiver_freeMsg(self->h_uartReceiver);
    }
}

void SetGains(uint8_t* cmdData)
{
    int32_t refGain = ((int32_t*)cmdData)[0];
    int32_t errGain = ((int32_t*)cmdData)[1];
    int32_t outGain = ((int32_t*)cmdData)[2];
    switch (refGain)
    {
        case 2:
        anc_gain_refSet(ANC_GAIN_2);
        break;
        case 4:
        anc_gain_refSet(ANC_GAIN_4);
        break;
        case 10:
        anc_gain_refSet(ANC_GAIN_10);
        break;
        case 20:
        anc_gain_refSet(ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Ref gain!");
        break;
    }
    switch (errGain)
    {
        case 2:
        anc_gain_errSet(ANC_GAIN_2);
        break;
        case 4:
        anc_gain_errSet(ANC_GAIN_4);
        break;
        case 10:
        anc_gain_errSet(ANC_GAIN_10);
        break;
        case 20:
        anc_gain_errSet(ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Err gain!");
        break;
    }
    switch (outGain)
    {
        case 2:
        anc_gain_outSet(ANC_GAIN_2);
        break;
        case 4:
        anc_gain_outSet(ANC_GAIN_4);
        break;
        case 10:
        anc_gain_outSet(ANC_GAIN_10);
        break;
        case 20:
        anc_gain_outSet(ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Out gain!");
        break;
    }
}

/* Callbacks for anc_acquisition module */

void acquisition_bfr0_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
)
{
    anc_processing_preprocessing_data_t inputSamples;
    q15_t out;

    performance_begin(&m_app.performance[0]);

    inputSamples = anc_processing_preprocessing(
        &m_app.ancProcessing[0],
        refMicBfr,
        errMicBfr,
        outDacBfr
    );

    out = anc_algorithm_calculate(
        &m_app.ancAlgorithm[0],
        inputSamples
    );

    anc_processing_postprocessing(
        &m_app.ancProcessing[0],
        inputSamples,
        out,
        outDacBfr
    );

    performance_end(&m_app.performance[0]);
}

void acquisition_bfr1_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
)
{
    anc_processing_preprocessing_data_t inputSamples;
    q15_t out;

    performance_begin(&m_app.performance[1]);

    inputSamples = anc_processing_preprocessing(
        &m_app.ancProcessing[1],
        refMicBfr,
        errMicBfr,
        outDacBfr
    );

    out = anc_algorithm_calculate(
        &m_app.ancAlgorithm[1],
        inputSamples
    );

    anc_processing_postprocessing(
        &m_app.ancProcessing[1],
        inputSamples,
        out,
        outDacBfr
    );

    performance_end(&m_app.performance[1]);
}

void offline_identification_bfr0_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
)
{
    anc_processing_preprocessing_data_t inputSamples;
    q15_t out;

    performance_begin(&m_app.performance[0]);

    inputSamples = anc_processing_preprocessing(
        &m_app.ancProcessing[0],
        refMicBfr,
        errMicBfr,
        outDacBfr
    );

    out = anc_offline_identification_calculate(
        &m_app.ancOfflineIdentification[0],
        inputSamples
    );

    anc_processing_postprocessing(
        &m_app.ancProcessing[0],
        inputSamples,
        out,
        outDacBfr
    );

    m_app.ancProcessingLogData.outSample = errFiltered;

    performance_end(&m_app.performance[0]);
}

void offline_identification_bfr1_callback(
    uint16_t* refMicBfr,
    uint16_t* errMicBfr,
    uint16_t* outDacBfr
)
{
    anc_processing_preprocessing_data_t inputSamples;
    q15_t out;

    performance_begin(&m_app.performance[1]);

    inputSamples = anc_processing_preprocessing(
        &m_app.ancProcessing[1],
        refMicBfr,
        errMicBfr,
        outDacBfr
    );

    out = anc_offline_identification_calculate(
        &m_app.ancOfflineIdentification[1],
        inputSamples
    );

    anc_processing_postprocessing(
        &m_app.ancProcessing[1],
        inputSamples,
        out,
        outDacBfr
    );

    m_app.ancProcessingLogData.outSample = errFiltered;

    performance_end(&m_app.performance[1]);
}

/* Private callbacks from other modules */

void uart_receiver_onQueueFullCallback(uart_receiver_t* self)
{
    SWO_LOG("Uart Receiver Queue full event!");
}

void anc_processing_onErrorCallback(anc_processing_t*   self)
{
    SWO_LOG("ANC processing error!");
    anc_acquisition_stop(m_app.h_ancAcquisition);
    m_app.state = ANC_APPLICATION_IDLE;
}

void anc_algorithm_onErrorCallback(anc_algorithm_t* self)
{
    SWO_LOG("ANC algorithm error!");
    anc_acquisition_stop(m_app.h_ancAcquisition);
    m_app.state = ANC_APPLICATION_IDLE;
}

void anc_offline_identification_onErrorCallback(
    anc_offline_identification_t* self
)
{
    SWO_LOG("ANC offline identification error!");
    anc_acquisition_stop(m_app.h_ancAcquisition);
    m_app.state = ANC_APPLICATION_IDLE;
}

void anc_offline_identification_onEndCallback(
    anc_offline_identification_t* self
)
{
    SWO_LOG("ANC offline identification finished.");
    anc_acquisition_stop(m_app.h_ancAcquisition);
    m_app.state = ANC_APPLICATION_IDLE;
}