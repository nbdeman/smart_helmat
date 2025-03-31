#include "MQ2.h"
float adc_value = 0;
float voltage = 0;
float RS = 0;
float RL = 4.7;
float R0 = 35.904;
float ppm = 0;
uint32_t dma_buffer[30];
_Bool c3h8_flag; // c3h8超标flag
void mq2_proc(void)
{
  adc_value = 0;
  for (uint8_t i = 0; i < 30; i++)
  {
    adc_value += (float)dma_buffer[i];
  }
  voltage = (float)(adc_value / 30.0f) / 4095 * 5.0f;
  //	printf("voltage:%f\r\n",voltage);
  RS = ((5.0f - voltage) / voltage) * RL;
  // 测丙烷：Rs / R0 = 11.5428 * ppm ^ (-0.6549)
  sensor.c3h8_ppm = (float)(pow((float)RS / ((float)R0 * 11.5428), -1.5278));
 // 丙烷浓度超标检测
  if (sensor.c3h8_ppm > 5250)
  {
    printf("beyond dog");
    event.ppm_flag = DETECTED;
  }
  //  printf("ppm%f\r\n", sensor.c3h8_ppm);
}
