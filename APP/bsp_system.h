#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "Config.h"
#include "main.h"
#include "usart.h"
#include "filter.h"
#include "scheduler.h"
#include "i2c.h"
#include "math.h"
#include "stdbool.h"
#include "adc.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "atgm336h_app.h"
#include "delay.h"
#include "usart.h"
extern uint8_t rx6_flag;
extern uint8_t rx4_flag;
// #include "sys.h"
#include "key_app.h"

#include "max30102_app.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "MPU6050.h"

#include "led_app.h"
#include "ci_app.h"
extern uint8_t ucled[2];
#include "ringbuffer.h"
extern uint8_t uart1_dma_buffer[128];
extern uint8_t uart2_dma_buffer[512];
extern uint8_t uart6_dma_buffer[1000];
extern uint8_t uart4_dma_buffer[40];
#include "dht11.h"

#include "MQ2.h"
extern uint32_t dma_buffer[30];
extern uint8_t send_time;
#include "esp_8266.h"

#endif
