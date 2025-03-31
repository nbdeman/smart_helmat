#include "scheduler.h"
EventDetection event; /*事件定义*/
Sensor_data sensor;

// 全局任务存储数组，用于存放任务信息
SchedulerContext scheduler_context;

// void scheduler_init(void)
//{
//     // 初始化任务数量
//     scheduler_context.adc_task_num = sizeof(adc_task) / sizeof(task_t);
//   //  scheduler_context.uart_task_num = sizeof(uart_task) / sizeof(task_t);
//   //  scheduler_context.mpu6050_task_num = sizeof(mpu6050_task) / sizeof(task_t);
// }
// void scheduler_run_tasks(task_t *tasks, uint8_t num_tasks, uint32_t current_time)
//{
//     for (uint8_t i = 0; i < num_tasks; i++)
//     {
//         uint32_t now_time = current_time;
//         if (now_time >= tasks[i].rate_ms + tasks[i].last_run)
//         {
//             tasks[i].last_run = now_time;
//             tasks[i].task_func();
//         }
//     }
// }
// void scheduler_run(void)
//{
//	//	scheduler_run_tasks(mpu6050_task, scheduler_context.mpu6050_task_num, scheduler_context.mpu6050_time);
//     scheduler_run_tasks(adc_task, scheduler_context.adc_task_num, scheduler_context.adc_time);
//   //  scheduler_run_tasks(uart_task, scheduler_context.uart_task_num, scheduler_context.uart_time);
// }

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//     if (htim == &htim2)
//     {
//         scheduler_context.adc_time++;
//     }
//     if (htim == &htim3)
//     {
//         scheduler_context.uart_time++;
//     }
//     if (htim == &htim4)
//     {
//         scheduler_context.mpu6050_time++;
//     }
// }
