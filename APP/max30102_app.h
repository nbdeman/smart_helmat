#ifndef __MAX30102_APP_H_
#define __MAX30102_APP_H_

#include "bsp_system.h"
#define I2C_Write_Addr 0xAE
#define I2C_Read_Addr 0xAF
// register addresses
#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

#define HEART_RATE_COMPENSATION 10 // ÐÄÂÊ²¹³¥Öµ£¬Ã¿´Î²¹³¥¹Ì¶¨µÄÐÄÂÊÖµ
#define WINDOW_SIZE 20             // »¬¶¯´°¿Ú´óÐ¡
#define ALPHA 0.05                 // µÍÍ¨ÂË²¨µÄÂË²¨ÏµÊý

#define HEART_RATE_COMPENSATION 10 // 心率补偿值，每次补偿固定的心率值

// 定义常量和数据结构
#define MAX_BRIGHTNESS 255
#define INTERRUPT_REG 0x00
#define BUFFER_LENGTH 500 // 数据缓冲区长度

// MAX30102 数据结构
typedef struct
{
    uint32_t ir_buffer[BUFFER_LENGTH];  // 红外 LED 数据（用于血氧计算）
    uint32_t red_buffer[BUFFER_LENGTH]; // 红光 LED 数据（用于心率计算）
    int32_t spO2;                       // 血氧饱和度
    int8_t spO2_valid;                  // 血氧有效性的标志
    int32_t heart_rate;                 // 心率
    int8_t heart_rate_valid;            // 心率有效性的标志
    int32_t min_value;                  // 信号最小值
    int32_t max_value;                  // 信号最大值
    int32_t prev_data;                  // 上一个数据点
    int32_t brightness;                 // 信号亮度（用于心率计算）
    uint32_t buffer_length;             // 数据缓冲区长度
} MAX30102_Data;
// 函数声明
void MAX30102_Read_Data(void);            // 从 MAX30102 读取数据
void Calculate_Heart_Rate_and_SpO2(void); // 计算心率和血氧饱和度
void Update_Signal_Min_Max(void);         // 更新信号的最小值和最大值
extern MAX30102_Data max30102_data;       // 外部声明的 MAX30102 数据结构
void Process_And_Display_Data(void);      // 处理并显示数据
void max30102_task(void);
// MAX30102操作函数
void MAX30102_Init(void);
void MAX30102_Reset(void);
uint8_t M30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data);
uint8_t max30102_Bus_Read(uint8_t Register_Address);
void max30102_FIFO_ReadWords(uint8_t Register_Address, uint16_t Word_Data[][2], uint8_t count);
HAL_StatusTypeDef max30102_FIFO_ReadBytes(uint8_t Register_Address, uint8_t *Data);
void max30102_abnormal_event(void);
// void maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data);
// void maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data);
// void maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led);
#endif
