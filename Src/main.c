/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "datalogger.h"
#include "anc_acquisition.h"
#include "math.h"
#include "iir.h"
#include "uart_receiver.h"
#include "swo_logger.h"
#include "anc_cmd.h"
#include "anc_gain.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ANC_ACQUISITION_CHUNK_SIZE    4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DataLogger_t      DataLogger;
anc_acquisition_t AncAcquisition;
uart_receiver_t   UartReceiver;

/* Normal IIR with notch for 50 Hz */
const float iir_b_coeffs[4] = {
  0.9883,
  -2.9635,
  2.9635,
  -0.9883
};
const float iir_a_coeffs[3] = {
  -2.9751,
  2.9518,
  -0.9767
};

/* IIR with notch for 500 Hz */
/*
const float iir_b_coeffs[4] = {
  0.9547,
  -2.7187,
  2.7187,
  -0.9547
};
const float iir_a_coeffs[3] = {
  -2.7623,
  2.6746,
  -0.9100
};
*/

iir_t IirRefMic;
iir_t IirErrMic;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  DataLogger_Log(&DataLogger);
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

void uart_receiver_onQueueFullCallback(uart_receiver_t* self)
{
  SWO_LOG("Uart Receiver Queue full event!");
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */


  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */


  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* System interrupt init*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  DataLogger_Init(&DataLogger);
  anc_acquisition_init(&AncAcquisition);
  uart_receiver_init(&UartReceiver);
  SWO_LOG_INIT();

  /* Set all gains to default */
  anc_gain_set(ANC_GAIN_REF_MIC, ANC_GAIN_2);
  anc_gain_set(ANC_GAIN_ERR_MIC, ANC_GAIN_2);
  anc_gain_set(ANC_GAIN_OUT_DAC, ANC_GAIN_2);

  iir_init(&IirRefMic, iir_b_coeffs, iir_a_coeffs);
  iir_init(&IirErrMic, iir_b_coeffs, iir_a_coeffs);

  anc_acquisition_configure(&AncAcquisition, ANC_ACQUISITION_CHUNK_SIZE,
    anc_acquisition_bfr0_callback, anc_acquisition_bfr1_callback);

  SWO_LOG("ANC started!");

  uart_receiver_start(&UartReceiver);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    char* rcvMsg_p;
    rcvMsg_p = uart_receiver_getMsg(&UartReceiver);
    if (rcvMsg_p != UART_RECEIVER_NO_MSG)
    {
      anc_cmd_t cmd = anc_cmd_decode(rcvMsg_p);
      switch (cmd)
      {
        case ANC_CMD_START:
          anc_acquisition_start(&AncAcquisition);
          break;
        case ANC_CMD_STOP:
          anc_acquisition_stop(&AncAcquisition);
          break;
        default:
          SWO_LOG("Unrecognized command.");
          break;
      }
      SWO_LOG("%s", rcvMsg_p);
      uart_receiver_freeMsg(&UartReceiver);
    }
    SWO_LOG_PROCESS();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
  {
  Error_Handler();
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 168, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_16);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(168000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(168000000);
}

/* USER CODE BEGIN 4 */

int __io_putchar(int ch)
{
  ITM_SendChar(ch);
  return ch;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
