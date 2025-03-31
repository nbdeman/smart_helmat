/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_system.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for MAXTask */
osThreadId_t MAXTaskHandle;
const osThreadAttr_t MAXTask_attributes = {
  .name = "MAXTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for MPU_Task */
osThreadId_t MPU_TaskHandle;
const osThreadAttr_t MPU_Task_attributes = {
  .name = "MPU_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal6,
};
/* Definitions for WIFE_Task */
osThreadId_t WIFE_TaskHandle;
const osThreadAttr_t WIFE_Task_attributes = {
  .name = "WIFE_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for MQ2_Task */
osThreadId_t MQ2_TaskHandle;
const osThreadAttr_t MQ2_Task_attributes = {
  .name = "MQ2_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for IO_Task */
osThreadId_t IO_TaskHandle;
const osThreadAttr_t IO_Task_attributes = {
  .name = "IO_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for DHT11_Task */
osThreadId_t DHT11_TaskHandle;
const osThreadAttr_t DHT11_Task_attributes = {
  .name = "DHT11_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for GPS_Task */
osThreadId_t GPS_TaskHandle;
const osThreadAttr_t GPS_Task_attributes = {
  .name = "GPS_Task",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for ci_Task10 */
osThreadId_t ci_Task10Handle;
const osThreadAttr_t ci_Task10_attributes = {
  .name = "ci_Task10",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal4,
};
/* Definitions for control_mode_Timer */
osTimerId_t control_mode_TimerHandle;
const osTimerAttr_t control_mode_Timer_attributes = {
  .name = "control_mode_Timer"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void MAX_StartTask(void *argument);
void MPU_StartTask(void *argument);
void WIFE_StartTask(void *argument);
void MQ2_StartTask(void *argument);
void IO_StartTask(void *argument);
void DHT11_StartTask(void *argument);
void GPS_StartTask(void *argument);
void ci_StartTask(void *argument);
void Timer1_Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of control_mode_Timer */
  control_mode_TimerHandle = osTimerNew(Timer1_Callback, osTimerPeriodic, NULL, &control_mode_Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  osTimerStart(control_mode_TimerHandle, 100);
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of MAXTask */
  MAXTaskHandle = osThreadNew(MAX_StartTask, NULL, &MAXTask_attributes);

  /* creation of MPU_Task */
  MPU_TaskHandle = osThreadNew(MPU_StartTask, NULL, &MPU_Task_attributes);

  /* creation of WIFE_Task */
  WIFE_TaskHandle = osThreadNew(WIFE_StartTask, NULL, &WIFE_Task_attributes);

  /* creation of MQ2_Task */
  MQ2_TaskHandle = osThreadNew(MQ2_StartTask, NULL, &MQ2_Task_attributes);

  /* creation of IO_Task */
  IO_TaskHandle = osThreadNew(IO_StartTask, NULL, &IO_Task_attributes);

  /* creation of DHT11_Task */
  DHT11_TaskHandle = osThreadNew(DHT11_StartTask, NULL, &DHT11_Task_attributes);

  /* creation of GPS_Task */
  GPS_TaskHandle = osThreadNew(GPS_StartTask, NULL, &GPS_Task_attributes);

  /* creation of ci_Task10 */
  ci_Task10Handle = osThreadNew(ci_StartTask, NULL, &ci_Task10_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_MAX_StartTask */
/**
 * @brief Function implementing the MAXTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MAX_StartTask */
void MAX_StartTask(void *argument)
{
  /* USER CODE BEGIN MAX_StartTask */
  MAX30102_Init();
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    max30102_task();
    //	printf("aaaa\r\n");
    //	 prevTicks += 1000;
    //  osDelayUntil(prevTicks);
    osDelay(3000);
  }
  /* USER CODE END MAX_StartTask */
}

/* USER CODE BEGIN Header_MPU_StartTask */
/**
 * @brief Function implementing the MPU_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MPU_StartTask */
void MPU_StartTask(void *argument)
{
  /* USER CODE BEGIN MPU_StartTask */
  printf("mpu_dmp_init\r\n");
  while (mpu_dmp_init())
  {
    printf("mpu_dmp_init again\r\n");
    NVIC_SystemReset();
  }
  //
  printf("mpu_dmp_init success\r\n");
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    dmp_proc();
    prevTicks += 50;
    osDelayUntil(prevTicks);
    //	osDelay(30);
  }
  /* USER CODE END MPU_StartTask */
}

/* USER CODE BEGIN Header_WIFE_StartTask */
/**
 * @brief Function implementing the WIFE_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_WIFE_StartTask */
void WIFE_StartTask(void *argument)
{
  /* USER CODE BEGIN WIFE_StartTask */

  esp8266_init();
  /* Infinite loop */
  for (;;)
  {
    uart_to_onenet();
    osDelay(5000);
  }
  /* USER CODE END WIFE_StartTask */
}

/* USER CODE BEGIN Header_MQ2_StartTask */
/**
 * @brief Function implementing the MQ2_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_MQ2_StartTask */
void MQ2_StartTask(void *argument)
{
  /* USER CODE BEGIN MQ2_StartTask */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&dma_buffer[0], 30);
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    mq2_proc();

    // printf("pitch:%f roll:%f yaw:%f\r\n",mpu_data.pitch,mpu_data.roll,mpu_data.yaw)
    prevTicks += 200;
    osDelayUntil(prevTicks);
    // osDelay(1);
  }
  /* USER CODE END MQ2_StartTask */
}

/* USER CODE BEGIN Header_IO_StartTask */
/**
 * @brief Function implementing the IO_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_IO_StartTask */
void IO_StartTask(void *argument)
{
  /* USER CODE BEGIN IO_StartTask */
  key_init();
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    key_proc();
    led_proc();
    prevTicks += 10;
    osDelayUntil(prevTicks);
  }
  /* USER CODE END IO_StartTask */
}

/* USER CODE BEGIN Header_DHT11_StartTask */
/**
 * @brief Function implementing the DHT11_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_DHT11_StartTask */
void DHT11_StartTask(void *argument)
{
  /* USER CODE BEGIN DHT11_StartTask */
  DHT11_Init();
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    dht11_proc();
    prevTicks += 200;
    osDelayUntil(prevTicks);
  }
  /* USER CODE END DHT11_StartTask */
}

/* USER CODE BEGIN Header_GPS_StartTask */
/**
 * @brief Function implementing the GPS_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_GPS_StartTask */
void GPS_StartTask(void *argument)
{
  /* USER CODE BEGIN GPS_StartTask */
  uint32_t prevTicks = osKernelGetTickCount(); // 获取当前滴答定时
  /* Infinite loop */
  for (;;)
  {
    atgm336h_proc();
    prevTicks += 500;
    osDelayUntil(prevTicks);
  }
  /* USER CODE END GPS_StartTask */
}

/* USER CODE BEGIN Header_ci_StartTask */
/**
 * @brief Function implementing the ci_Task10 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_ci_StartTask */
void ci_StartTask(void *argument)
{
  /* USER CODE BEGIN ci_StartTask */
  MPU6050_Init();
  /* Infinite loop */
  for (;;)
  {
    ci_proc();
    osDelay(300);
  }
  /* USER CODE END ci_StartTask */
}

/* Timer1_Callback function */
void Timer1_Callback(void *argument)
{
  /* USER CODE BEGIN Timer1_Callback */
  static uint8_t time = 0;
  if (event.control_mode == DETECTED)
  {
    time++;
    //	 printf("send_time:%d\r\n", time);
    if (time >= 100)
    {
      time = 0;

      event.control_mode = NOT_DETECTED;
    }
  }
  if (event.ci_send_flag == DETECTED)
  {
    send_time++;
  //  printf("send_time:%d\r\n", send_time);
  }
  /* USER CODE END Timer1_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

