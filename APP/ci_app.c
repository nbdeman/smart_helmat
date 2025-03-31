
#include "ci_app.h"

// UART1_RX串口通信消息头
const unsigned char g_uart_send_head_UART1_RX[] = {
    0xaa, 0x55};

// UART1_RX串口通信消息尾
const unsigned char g_uart_send_foot_UART1_RX[] = {
    0x55, 0xaa};

// 串口发送函数实现
void _uart_send_impl(unsigned char *buff, int len)
{
  HAL_UART_Transmit_DMA(&huart4, buff, len);
  osDelay(20);
}
// 十六位整数转32位整数
void _int16_to_int32(uart_param_t *param)
{
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = (value >> 15) & 1;
  unsigned long v = value;
  if (sign)
    v = 0xFFFF0000 | value;
  uart_param_t p;
  p.d_long = v;
  param->d_ucs[0] = p.d_ucs[0];
  param->d_ucs[1] = p.d_ucs[1];
  param->d_ucs[2] = p.d_ucs[2];
  param->d_ucs[3] = p.d_ucs[3];
}

// 浮点数转双精度
void _float_to_double(uart_param_t *param)
{
  if (sizeof(int) >= 4)
    return;
  unsigned long value = param->d_long;
  unsigned long sign = value >> 31;
  unsigned long M = value & 0x007FFFFF;
  unsigned long e = ((value >> 23) & 0xFF) - 127 + 1023;
  uart_param_t p0, p1;
  p1.d_long = ((sign & 1) << 31) | ((e & 0x7FF) << 20) | (M >> 3);
  p0.d_long = (M & 0x7) << 29;
  param->d_ucs[0] = p0.d_ucs[0];
  param->d_ucs[1] = p0.d_ucs[1];
  param->d_ucs[2] = p0.d_ucs[2];
  param->d_ucs[3] = p0.d_ucs[3];
  param->d_ucs[4] = p1.d_ucs[0];
  param->d_ucs[5] = p1.d_ucs[1];
  param->d_ucs[6] = p1.d_ucs[2];
  param->d_ucs[7] = p1.d_ucs[3];
}
// action: Read_SPO2_heart
void _uart_Read_SPO2_heart(int spo2, int heart)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_Read_SPO2_heart;
  param.d_int = spo2;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  param.d_int = heart;
  _int16_to_int32(&param);
  buff[7] = param.d_ucs[0];
  buff[8] = param.d_ucs[1];
  buff[9] = param.d_ucs[2];
  buff[10] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 11] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 13);
}

// action: Read_MQ2
void _uart_Read_MQ2(int MQ2_PPM)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_Read_MQ2;
  param.d_int = MQ2_PPM;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}

// action: Read_DHT11
void _uart_Read_DHT11(double TEMP, int Humi)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_Read_DHT11;
  param.d_double = TEMP;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  param.d_int = Humi;
  _int16_to_int32(&param);
  buff[11] = param.d_ucs[0];
  buff[12] = param.d_ucs[1];
  buff[13] = param.d_ucs[2];
  buff[14] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 15] = g_uart_send_foot_UART1_RX[i];
  }
  printf("%s", buff);
  _uart_send_impl(buff, 17);
}

// action: body_fall
void _uart_body_fall(int Fall_flag)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_body_fall;
  param.d_int = Fall_flag;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}

// action: body_beyond
void _uart_body_beyond(int body_flag)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_body_beyond;
  param.d_int = body_flag;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}

// action: MQ2_Beyond
void _uart_MQ2_Beyond(int mq2_beyond)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_MQ2_Beyond;
  param.d_int = mq2_beyond;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}

// action: Temp_Beyond
void _uart_Temp_Beyond(int T_by)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_Temp_Beyond;
  param.d_int = T_by;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}
#define AVM_LOWER_LIMIT 5000
#define AVM_HIGHER_LIMIT 14000
#define GVM_THRESHOULD 100
#define FALL_THRESHOULD 60
void fall_collsion_event(void)
{
    MPU_Get_Gyroscope(&mpu_data);
    MPU_Get_Accelerometer(&mpu_data);

   // 	printf("ax:%d,ay%d,az%d\r\n",mpu_data.ax,mpu_data.ay,mpu_data.az);
    mpu_data.AVM = sqrt(pow(mpu_data.ax, 2) + pow(mpu_data.ay, 2) + pow(mpu_data.az, 2));
    mpu_data.GVM = sqrt(pow(mpu_data.gx, 2) + pow(mpu_data.gy, 2) + pow(mpu_data.gz, 2));
       printf("AVM:%d GVM:%d pitch:%.2f roll:%.2f\r\n", mpu_data.AVM, mpu_data.GVM, mpu_data.pitch, mpu_data.roll);
		if(mpu_data.pitch !=0 && mpu_data.roll != 0 &&mpu_data.AVM != 0 && mpu_data.GVM != 0){
			    if ((fabs(mpu_data.pitch) > FALL_THRESHOULD || fabs(mpu_data.roll) > FALL_THRESHOULD) && (mpu_data.AVM < AVM_LOWER_LIMIT || mpu_data.AVM > AVM_HIGHER_LIMIT) && mpu_data.GVM > GVM_THRESHOULD)
    {
      event.fall_flag = DETECTED;
      printf("fall_flag\r\n");
    }
    if (mpu_data.AVM < AVM_LOWER_LIMIT || mpu_data.AVM  > AVM_HIGHER_LIMIT)
    {
      event.collision_flag = DETECTED;
      printf("collision_flag\r\n");
    }
		}

}
// action: wear_flag
void _uart_wear_flag(int wear_flag)
{
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN_UART1_RX; i++)
  {
    buff[i + 0] = g_uart_send_head_UART1_RX[i];
  }
  buff[2] = U_MSG_wear_flag;
  param.d_int = wear_flag;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN_UART1_RX; i++)
  {
    buff[i + 7] = g_uart_send_foot_UART1_RX[i];
  }
  _uart_send_impl(buff, 9);
}

uint8_t send_time = 0;
void ci_proc(void)
{
  fall_collsion_event();
  if (uart4_dma_buffer[0] == 0x11)
  {
    _uart_Read_SPO2_heart(sensor.spO2, sensor.heart_rate);
    printf("aaaa\r\n");
    memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
  }
  if (uart4_dma_buffer[0] == 0x22)
  {
    _uart_Read_MQ2(sensor.c3h8_ppm);
    memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
  }
  if (uart4_dma_buffer[0] == 0x33)
  {
    _uart_Read_DHT11(sensor.temp, sensor.humi);
    memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
  }
  if (uart4_dma_buffer[0] == 0x44)
  {
    event.sos = DETECTED;
    memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
  }
  if (uart4_dma_buffer[0] == 0x55)
  {
    event.collision_flag = NOT_DETECTED;
    event.fall_flag = NOT_DETECTED;
    if (sensor.heart_rate >= 60 && sensor.heart_rate <= 100)
    {
      event.heart_flag = NOT_DETECTED;
    }
    if (sensor.spO2 >= 95 && sensor.spO2 <= 100)
    {
      event.spo2_flag = NOT_DETECTED;
    }
    if (sensor.c3h8_ppm <= 5250)
    {
      event.ppm_flag = NOT_DETECTED;
    }
    event.ci_send_flag = NOT_DETECTED;
    event.control_mode = DETECTED;
    memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
  }
  if (uart4_dma_buffer[0] == 0x66)
  {
    ucled[0] = 0;
  }
  if (uart4_dma_buffer[0] == 0x77)
  {
    ucled[0] = 1;
  }
  if (event.control_mode == NOT_DETECTED)
  {
    if (event.ppm_flag == DETECTED || event.fall_flag == DETECTED || event.collision_flag == DETECTED || event.wear_flag == DETECTED)
    {
      event.ci_send_flag = DETECTED;
      // printf("aaaaaaaaaaaaaaaa\r\n");
    }
    if (send_time >= 25)
    {

      if (event.fall_flag == DETECTED || event.collision_flag == DETECTED)
      {
        _uart_body_fall(event.fall_flag);
      }
      if (event.ppm_flag == DETECTED)
      {
        _uart_MQ2_Beyond(event.ppm_flag);
      }
      if (event.wear_flag == DETECTED)
      {
        // printf("aaaaaaaaaaaaa\r\n");
        _uart_wear_flag(event.wear_flag);
      }
      send_time = 0;
    }
     if (event.heart_flag == DETECTED || event.spo2_flag == DETECTED)
     {
       _uart_body_beyond(event.heart_flag);
     }
  }
  memset(uart4_dma_buffer, 0, sizeof(uart4_dma_buffer));
}
