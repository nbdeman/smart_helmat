#include "filter.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief 限制数组中的数值范围，确保每个值都在指定的最小值和最大值之间。
 *
 * @param data 数组的指针
 * @param size 数组的大小
 * @param min_val 最小允许值
 * @param max_val 最大允许值
 */
void limit_value(uint32_t *data, int size, uint32_t min_val, uint32_t max_val)
{
    for (int i = 0; i < size; i++)
    {
        if (data[i] < min_val)
        {
            data[i] = min_val; // 如果值小于最小值，将其限制为最小值
        }
        else if (data[i] > max_val)
        {
            data[i] = max_val; // 如果值大于最大值，将其限制为最大值
        }
    }
}

/**
 * @brief 用于qsort的比较函数，比较两个uint32_t类型的值。
 *
 * @param a 第一个比较的元素
 * @param b 第二个比较的元素
 * @return int 返回值小于0表示a小于b，等于0表示a等于b，大于0表示a大于b
 */
int compare(const void *a, const void *b)
{
    return (*(uint32_t *)a - *(uint32_t *)b); // 比较两个uint32_t类型的值
}

/**
 * @brief 计算数组的中位数。
 *
 * @param data 数组的指针
 * @param size 数组的大小
 * @return uint32_t 返回数组的中位数
 */
uint32_t mid_value(uint32_t *data, int size)
{
    uint32_t *copy = (uint32_t *)malloc(size * sizeof(uint32_t)); // 分配临时数组用于排序
    memcpy(copy, data, size * sizeof(uint32_t));                  // 复制原数组到临时数组
    qsort(copy, size, sizeof(uint32_t), compare);                 // 使用qsort对临时数组排序
    uint32_t median = (size % 2 == 0) ? (copy[size / 2 - 1] + copy[size / 2]) / 2 : copy[size / 2];
    // 如果数组大小为偶数，中位数为中间两个数的平均值；否则为中间的数
    free(copy); // 释放临时数组
    return median;
}

/**
 * @brief 计算数组的平均值。
 *
 * @param data 数组的指针
 * @param size 数组的大小
 * @return uint32_t 返回数组的平均值
 */
uint32_t avg_value(uint32_t *data, int size)
{
    uint32_t sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += data[i]; // 累加数组中的所有值
    }
    return sum / size; // 返回平均值
}

/**
 * @brief 一阶低通滤波器，用于平滑ADC采样值。
 *
 * @param CurrValue 当前采样值
 * @return uint32_t 滤波后的值
 */
uint32_t adc_filter(uint32_t CurrValue)
{
    static uint32_t LastValue = 0;                         // 上一次滤波后的值，初始为0
    uint32_t tmp = (32 * CurrValue + 96 * LastValue) >> 7; // 滤波公式
    LastValue = tmp;                                       // 更新上次滤波值
    return LastValue;                                      // 返回滤波后的值
}
/********************************** 滤波算法的全局变量 *************************************************/

/********************************** 滤波算法 *************************************************/

// 滑动平均滤波函数
int SmoothData(int new_value, int *buffer, int *index)
{
    buffer[*index] = new_value;          // 更新滑动窗口
    *index = (*index + 1) % WINDOW_SIZE; // 循环索引

    int sum = 0;
    for (int i = 0; i < WINDOW_SIZE; i++)
    {
        sum += buffer[i]; // 计算窗口内的平均值
    }

    return sum / WINDOW_SIZE; // 返回平均值
}

// 低通滤波函数
int LowPassFilter(int new_value, int previous_filtered_value)
{
    return (int)(ALPHA * new_value + (1 - ALPHA) * previous_filtered_value); // 按公式进行滤波
}

/********************************** 滤波算法 *************************************************/
