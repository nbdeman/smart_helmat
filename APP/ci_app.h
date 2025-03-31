#ifndef __CI_APP_H_
#define __CI_APP_H_
#include "bsp_system.h"
// 串口发送消息最大长度
#define UART_SEND_MAX 32
#define UART_MSG_HEAD_LEN_UART1_RX 2
#define UART_MSG_FOOT_LEN_UART1_RX 2

// 串口发送消息号
#define U_MSG_Read_SPO2_heart 1
#define U_MSG_Read_MQ2 2
#define U_MSG_Read_DHT11 3
#define U_MSG_body_fall 4
#define U_MSG_body_beyond 5
#define U_MSG_MQ2_Beyond 6
#define U_MSG_Temp_Beyond 7
#define U_MSG_wear_flag 8

// 串口消息参数类型
typedef union
{
  double d_double;
  int d_int;
  unsigned char d_ucs[8];
  char d_char;
  unsigned char d_uchar;
  unsigned long d_long;
  short d_short;
  float d_float;
} uart_param_t;

// action: Read_SPO2_heart
void _uart_Read_SPO2_heart(int spo2, int heart);

// action: Read_MQ2
void _uart_Read_MQ2(int MQ2_PPM);

// action: Read_DHT11
void _uart_Read_DHT11(double TEMP, int Humi);

// action: body_fall
void _uart_body_fall(int Fall_flag);

// action: body_beyond
void _uart_body_beyond(int body_flag);

// action: MQ2_Beyond
void _uart_MQ2_Beyond(int mq2_beyond);

// action: Temp_Beyond
void _uart_Temp_Beyond(int T_by);
// action: wear_flag
void _uart_wear_flag(int wear_flag);
void ci_proc(void);
#endif /*JX_UART_SEND_H_*/
