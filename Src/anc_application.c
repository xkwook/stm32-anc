/*
 * anc_application.c
 *
 *  Created on: Aug 8, 2020
 *      Author: klukomski
 */

#include "anc_application.h"

#include "anc_cmd.h"
#include "anc_gain.h"

#include "swo_logger.h"

/* Rubbish functions BEGIN */

#include "anc_parameters.h"
#include "datalogger.h"
#include "math.h"
#include "iir.h"

#define ANC_ACQUISITION_CHUNK_SIZE    4

iir_t IirRefMic;
iir_t IirErrMic;

DataLogger_t        DataLogger;

anc_application_t*  m_h_ancApplication;

static inline void rubbish_init(anc_application_t* self)
{
  iir_init(&IirRefMic, anc_iir_b_coeffs, anc_iir_a_coeffs);
  iir_init(&IirErrMic, anc_iir_b_coeffs, anc_iir_a_coeffs);

  m_h_ancApplication = self;
}

static inline void average_and_send(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr)
{
  static int32_t triangleCnt = 0;
  static int32_t triangleDir = 250;

  float* refMicFiltered;
  float* errMicFiltered;

  refMicFiltered = iir_perform(&IirRefMic, (int16_t*)refMicBfr);
  errMicFiltered = iir_perform(&IirErrMic, (int16_t*)errMicBfr);

  float refMicMean = 0.0;
  float errMicMean = 0.0;
  int32_t outDacMean = 0;
  for (uint32_t i = 0; i < ANC_ACQUISITION_CHUNK_SIZE; i++)
  {
    //refMicMean += refMicBfr[i];
    refMicMean += refMicFiltered[i];
    errMicMean += errMicFiltered[i];
    outDacMean += outDacBfr[i];
  }
  refMicMean /= ANC_ACQUISITION_CHUNK_SIZE;
  errMicMean /= ANC_ACQUISITION_CHUNK_SIZE;
  outDacMean /= ANC_ACQUISITION_CHUNK_SIZE;
  outDacMean -= ANC_DAC_OFFSET;

  /* Generate new DAC samples */
  /* Triangle of tone 500 Hz */
  for (uint32_t i = 0; i < ANC_ACQUISITION_CHUNK_SIZE; i++)
  {
    outDacBfr[i] = triangleCnt + ANC_DAC_OFFSET;
    triangleCnt += triangleDir;
  }
  if (triangleCnt == 2000 || triangleCnt == -2000)
  {
    triangleDir *= -1;
  }

  DataLogger.bfr.sample[0][0] = (int8_t)(refMicMean / 16.0);
  DataLogger.bfr.sample[1][0] = (int8_t)(errMicMean / 16.0);
  DataLogger.bfr.sample[2][0] = (int8_t)(outDacMean / 16);

  uart_transmitter_setMsg(m_h_ancApplication->h_uartTransmitter,
    (uint8_t*)&DataLogger.bfr, sizeof(DataLogger.bfr));
  uart_transmitter_send(m_h_ancApplication->h_uartTransmitter);
}

void anc_acquisition_bfr0_callback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr)
{
  average_and_send(refMicBfr, errMicBfr, outDacBfr);
}

void anc_acquisition_bfr1_callback(uint16_t* refMicBfr, uint16_t* errMicBfr, uint16_t* outDacBfr)
{
  static uint32_t cnt = 0;
  if (cnt % 1000 == 0)
  {
    LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
  cnt++;
  average_and_send(refMicBfr, errMicBfr, outDacBfr);
}

void SetGains(anc_application_t* self, uint8_t* cmdData)
{
    int32_t refGain = ((int32_t*)cmdData)[0];
    int32_t errGain = ((int32_t*)cmdData)[1];
    int32_t outGain = ((int32_t*)cmdData)[2];
    switch (refGain)
    {
        case 2:
        anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_2);
        break;
        case 4:
        anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_4);
        break;
        case 10:
        anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_10);
        break;
        case 20:
        anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Ref gain!");
        break;
    }
    switch (errGain)
    {
        case 2:
        anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_2);
        break;
        case 4:
        anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_4);
        break;
        case 10:
        anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_10);
        break;
        case 20:
        anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Err gain!");
        break;
    }
    switch (outGain)
    {
        case 2:
        anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_2);
        break;
        case 4:
        anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_4);
        break;
        case 10:
        anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_10);
        break;
        case 20:
        anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_20);
        break;
        default:
        SWO_LOG("Wrong Out gain!");
        break;
    }
}

/* Rubbish functions END */


/* Private methods declaration */

static inline void IdleStateHandle(anc_application_t* self);
static inline void AcquisitionStateHandle(anc_application_t* self);
static inline void IdentificationStateHandle(anc_application_t* self);
static inline void OfflineIdentificationStateHandle(anc_application_t* self);

/* Public methods definition */

void anc_application_init(
    anc_application_t*  self,
    anc_acquisition_t*  h_ancAcquisition,
    uart_receiver_t*    h_uartReceiver,
    uart_transmitter_t* h_uartTransmitter,
    identification_t*   h_identification
)
{
    self->h_ancAcquisition  = h_ancAcquisition;
    self->h_uartReceiver    = h_uartReceiver;
    self->h_uartTransmitter = h_uartTransmitter;
    self->h_identification  = h_identification;

    /* Initialize to IDLE (stopped) state */
    self->state = ANC_APPLICATION_IDLE;
    self->identificationState = ANC_APPLICATION_IDENTIFICATION_EMPTY;

    /* Set all gains to default */
    anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_2);
    anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_2);
    anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_2);

    rubbish_init(self);
}

void anc_application_start(anc_application_t* self)
{
    SWO_LOG("ANC application started!");
    uart_receiver_start(self->h_uartReceiver);

    /* Infinite loop */
    for (;;)
    {
        switch (self->state)
        {
        case ANC_APPLICATION_IDLE:
            IdleStateHandle(self);
            break;

        case ANC_APPLICATION_ACQUISITION:
            AcquisitionStateHandle(self);
            break;

        case ANC_APPLICATION_IDENTIFICATION:
            IdentificationStateHandle(self);
            break;

        case ANC_APPLICATION_OFFLINE_IDENTIFICATION:
            OfflineIdentificationStateHandle(self);
            break;

        default:
            break;
        }
        /* Process logs in the mean time */
        SWO_LOG_PROCESS();
    }
}

anc_application_state_t anc_application_getState(anc_application_t* self)
{
    return self->state;
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
            anc_acquisition_configure(self->h_ancAcquisition, ANC_ACQUISITION_CHUNK_SIZE,
                anc_acquisition_bfr0_callback, anc_acquisition_bfr1_callback);
            anc_acquisition_start(self->h_ancAcquisition);
            self->state = ANC_APPLICATION_ACQUISITION;
            break;
        case ANC_CMD_STOP:
            SWO_LOG("Already stopped!");
            break;
        case ANC_CMD_ANC_ON:
            break;
        case ANC_CMD_ANC_OFF:
            break;
        case ANC_CMD_AGC_ON:
            break;
        case ANC_CMD_AGC_OFF:
            break;
        case ANC_CMD_SET_GAINS:
            SetGains(self, cmdData);
            break;

        /* Only in Idle state commands */
        case ANC_CMD_IDENTIFICATION:
            identification_configure(self->h_identification,
                stabilizingCycles, sumCycles);
            LL_mDelay(mDelay);
            identification_start(self->h_identification);
            self->state = ANC_APPLICATION_IDENTIFICATION;
            break;
        case ANC_CMD_IDENTIFICATION_GET_REF:
            bfr = self->h_identification->refMicSum;
            if (self->identificationState == ANC_APPLICATION_IDENTIFICATION_READY)
            {
                uart_transmitter_setMsg(self->h_uartTransmitter,
                    (uint8_t*)bfr, IDENTIFICATION_BFR_LENGTH * sizeof(bfr[0]));
                uart_transmitter_send(self->h_uartTransmitter);
            }
            break;
        case ANC_CMD_IDENTIFICATION_GET_ERR:
            bfr = self->h_identification->errMicSum;
            if (self->identificationState == ANC_APPLICATION_IDENTIFICATION_READY)
            {
                uart_transmitter_setMsg(self->h_uartTransmitter,
                    (uint8_t*)bfr, IDENTIFICATION_BFR_LENGTH * sizeof(bfr[0]));
                uart_transmitter_send(self->h_uartTransmitter);
            }
            break;
        case ANC_CMD_OFFLINE_IDENTIFICATION:
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
            break;
        case ANC_CMD_ANC_OFF:
            break;
        case ANC_CMD_AGC_ON:
            break;
        case ANC_CMD_AGC_OFF:
            break;
        case ANC_CMD_SET_GAINS:
            SetGains(self, cmdData);
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
    if (identification_isDone(self->h_identification))
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

        default:
            SWO_LOG("Unsupported command.");
            break;
        }
        SWO_LOG("%s", rcvMsg_p);
        uart_receiver_freeMsg(self->h_uartReceiver);
    }
}

/* Private callbacks from other modules */

void uart_receiver_onQueueFullCallback(uart_receiver_t* self)
{
    SWO_LOG("Uart Receiver Queue full event!");
}