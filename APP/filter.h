#ifndef FILTER_H
#define FILTER_H

#include "bsp_system.h"
#define WINDOW_SIZE 20             // �������ڴ�С
#define ALPHA 0.05                 // ��������
uint32_t adc_filter(uint32_t value);
void limit_value(uint32_t *data, int size, uint32_t min_val, uint32_t max_val);
int compare(const void *a, const void *b);
uint32_t mid_value(uint32_t *data, int size);
uint32_t avg_value(uint32_t *data, int size);
uint32_t adc_filter(uint32_t CurrValue);
int SmoothData(int new_value, int *buffer, int *index);
int LowPassFilter(int new_value, int previous_filtered_value);
#endif
