#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "bsp_system.h"
/*传感器数据*/
typedef struct
{
    float temp;         // 温度
    uint8_t humi;       // 湿度
    int32_t spO2;       // 血氧浓度
    int32_t heart_rate; // 心率
    float longitude;    // 经度
    float latitude;     // 纬度
    float c3h8_ppm;     // c3h8浓度
} Sensor_data;

extern Sensor_data sensor;
typedef struct
{
    uint8_t adc_task_num;
    uint8_t uart_task_num;
    uint8_t mpu6050_task_num;
    uint32_t adc_time;
    uint32_t uart_time;
    uint32_t mpu6050_time;
} SchedulerContext;
extern SchedulerContext scheduler_context;
/*事件枚举*/
typedef enum
{
    DETECTED = 1,    // 检测成功
    NOT_DETECTED = 0 // 检测不成功
} DetectionStatus;
/*事件结构体*/
typedef struct
{
    DetectionStatus wear_flag;      // 佩戴检测
    DetectionStatus spo2_flag;      // 血氧检测
    DetectionStatus heart_flag;     // 心率检测
    DetectionStatus fall_flag;      // 跌倒检测
    DetectionStatus ppm_flag;       // 浓度检测
    DetectionStatus collision_flag; // 撞击检测
    DetectionStatus control_mode;
    DetectionStatus sos;
	 DetectionStatus ci_send_flag;
} EventDetection;

extern EventDetection event;

typedef struct
{
    void (*task_func)(void);
    uint32_t rate_ms;
    uint32_t last_run;
} task_t;
// void scheduler_run_tasks(task_t *tasks, uint8_t num_tasks, uint32_t current_time);
// extern task_t adc_task[3];
// void scheduler_init(void);
// void scheduler_run(void);

#endif
