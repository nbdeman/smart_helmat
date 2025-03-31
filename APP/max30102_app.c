#include "max30102_app.h"
#include "algorithm.h"
#define WEAR_THRESHOLD 10000 // 佩戴检测阈值
HAL_StatusTypeDef max30102_Bus_Write(uint8_t Register_Address, uint8_t Word_Data)
{
    if (HAL_I2C_Mem_Write(&hi2c3, I2C_Write_Addr, Register_Address, I2C_MEMADD_SIZE_8BIT, NULL, 0, HAL_MAX_DELAY) != HAL_OK)
    {

        printf("i2c3 error\r\n");
    }
    return HAL_I2C_Mem_Write(&hi2c3, I2C_Write_Addr, Register_Address, I2C_MEMADD_SIZE_8BIT, &Word_Data, 1, HAL_MAX_DELAY);
}
uint8_t max30102_Bus_Read(uint8_t Register_Address)
{
    uint8_t data;
    if (HAL_I2C_Mem_Read(&hi2c3, I2C_Read_Addr, Register_Address, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY) == HAL_OK)
    {
        return data;
    }
    else
    {
        printf("i2c3 error\r\n");
        return 1;
    }
}
// 从 MAX30102 的 FIFO 中读取数据，并将其存储到缓冲区中。
void max30102_FIFO_ReadWords(uint8_t Register_Address, uint16_t Word_Data[][2], uint8_t count)
{
    uint8_t i = 0;
    uint8_t no = count;
    uint8_t data[4]; // 用于暂存读取的字节数据

    // 第一步：向寄存器地址发送写命令（启动读取）
    if (HAL_I2C_Mem_Write(&hi2c3, I2C_Write_Addr, Register_Address, I2C_MEMADD_SIZE_8BIT, NULL, 0, HAL_MAX_DELAY) != HAL_OK)
    {
        printf("i2c2 error\r\n");
        // 写操作失败，返回
        return;
    }

    // 第二步：从 FIFO 寄存器读取数据
    while (no)
    {
        // 从 FIFO 寄存器读取 4 个字节（2 个 16 位数据）
        if (HAL_I2C_Master_Receive(&hi2c3, I2C_Read_Addr, data, 4, 100) != HAL_OK)
        {
            printf("i2c2 error\r\n");
            // 读取失败，返回
            return;
        }

        // 将读取的字节数据组合成 16 位数据
        Word_Data[i][0] = (uint16_t)(data[0] << 8) | data[1];
        Word_Data[i][1] = (uint16_t)(data[2] << 8) | data[3];

        no--;
        i++;
    }
}
HAL_StatusTypeDef max30102_FIFO_ReadBytes(uint8_t Register_Address, uint8_t *Data)
{
    max30102_Bus_Read(REG_INTR_STATUS_1);
    max30102_Bus_Read(REG_INTR_STATUS_2);
    // 第一步：向寄存器地址发送写命令（启动读取）
    if (HAL_I2C_Mem_Write(&hi2c3, I2C_Write_Addr, Register_Address, I2C_MEMADD_SIZE_8BIT, NULL, 0, 100) != HAL_OK)
    {
        printf("i2c2 error\r\n");
        return HAL_ERROR; // 写操作失败
    }

    // 第二步：从指定寄存器读取数据
    if (HAL_I2C_Master_Receive(&hi2c3, I2C_Read_Addr, Data, 6, 100) != HAL_OK)
    {
        printf("i2c2 error\r\n");
        return HAL_ERROR; // 读操作失败
    }

    return HAL_OK; // 操作成功
}
// 对 MAX30102 传感器进行初始化，设置工作模式、中断、FIFO 等参数。
void MAX30102_Reset(void)
{
    max30102_Bus_Write(REG_MODE_CONFIG, 0x40);
    max30102_Bus_Write(REG_MODE_CONFIG, 0x40);
}

void MAX30102_Init(void)
{
    MAX30102_Reset(); // 复位 MAX30102

    max30102_Bus_Write(REG_INTR_ENABLE_1, 0xc0); // 中断设置：使能 ALC 和 PPG 中断
    max30102_Bus_Write(REG_INTR_ENABLE_2, 0x00); // 中断设置：不使能其他中断
    max30102_Bus_Write(REG_FIFO_WR_PTR, 0x00);   // FIFO 写指针清零
    max30102_Bus_Write(REG_OVF_COUNTER, 0x00);   // 溢出计数器清零
    max30102_Bus_Write(REG_FIFO_RD_PTR, 0x00);   // FIFO 读指针清零
    max30102_Bus_Write(REG_FIFO_CONFIG, 0x0f);   // FIFO 配置：采样平均值为 1，FIFO 不回绕，FIFO 几乎满时为 17 个样本
    max30102_Bus_Write(REG_MODE_CONFIG, 0x03);   // 模式配置：0x02 为仅红光模式，0x03 为 SpO2 模式，0x07 为多模式 LED
    max30102_Bus_Write(REG_SPO2_CONFIG, 0x27);   // SpO2 配置：ADC 范围为 4096nA，采样率为 100 Hz，LED 脉冲宽度为 400us
    max30102_Bus_Write(REG_LED1_PA, 0x24);       // 设置 LED1 的电流为约 7mA
    max30102_Bus_Write(REG_LED2_PA, 0x24);       // 设置 LED2 的电流为约 7mA
    max30102_Bus_Write(REG_PILOT_PA, 0x7f);      // 设置 Pilot LED 的电流为约 25mA
    printf("successful\r\n");
}

// 初始化数据结构体
MAX30102_Data max30102_data = {
    .buffer_length = BUFFER_LENGTH, // 初始化缓冲区长度
    .min_value = 0x3FFFF,           // 初始化最小值
    .max_value = 0,                 // 初始化最大值
    .brightness = 0                 // 初始化亮度值
};
/********************************** 滤波算法的全局变量 *************************************************/

// 滑动平均滤波器缓冲区
int hr_buffer[WINDOW_SIZE] = {0};   // 用于心率的滑动窗口
int spo2_buffer[WINDOW_SIZE] = {0}; // 用于血氧的滑动窗口
int hr_index = 0, spo2_index = 0;   // 缓冲区索引

// 低通滤波器的先前值
int prev_hr = 0, prev_spo2 = 0; // 上一次的心率和血氧值
//// 从 MAX30102 读取数据，并进行心率补偿
void MAX30102_Read_Data(void)
{
    volatile uint32_t un_prev_data;
    uint8_t temp[6];
    // 读取前 500 个样本，确定信号范围
    for (int i = 0; i < max30102_data.buffer_length; i++)
    {
        while (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_7) == SET)
            ;
        // 等待中断信号
        max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp); // 从 MAX30102 FIFO 缓存区中读取数据
        max30102_data.red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2];
        max30102_data.ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];
        // 根据相邻两个 AD 数据的偏差确定 LED 亮度
        if (max30102_data.min_value > max30102_data.red_buffer[i])
            max30102_data.min_value = max30102_data.red_buffer[i];
        if (max30102_data.max_value < max30102_data.red_buffer[i])
            max30102_data.max_value = max30102_data.red_buffer[i];
    }
    // 更新上一个数据点
    un_prev_data = max30102_data.red_buffer[max30102_data.buffer_length - 1];

    // 调整补偿值：减少补偿并应用于心率计算前
    max30102_data.heart_rate += HEART_RATE_COMPENSATION; // 可减少补偿值
                                                         //  printf("aadddda\r\n");
}

// 计算心率和血氧值
void Calculate_Heart_Rate_and_SpO2(void)
{
    maxim_heart_rate_and_oxygen_saturation(max30102_data.ir_buffer, max30102_data.buffer_length,
                                           max30102_data.red_buffer, &max30102_data.spO2, &max30102_data.spO2_valid,
                                           &max30102_data.heart_rate, &max30102_data.heart_rate_valid);
    // printf("aaaaaaa\r\n");
}

// 更新信号的最小值和最大值，并应用滤波
void Update_Signal_Min_Max(void)
{
    uint32_t un_prev_data = max30102_data.red_buffer[max30102_data.buffer_length - 1];

    for (int i = 100; i < max30102_data.buffer_length; i++)
    {
        // 移动数据
        max30102_data.red_buffer[i - 100] = max30102_data.red_buffer[i];
        max30102_data.ir_buffer[i - 100] = max30102_data.ir_buffer[i];

        // 低通滤波处理
        max30102_data.red_buffer[i - 100] = LowPassFilter(max30102_data.red_buffer[i - 100], un_prev_data);
        max30102_data.ir_buffer[i - 100] = LowPassFilter(max30102_data.ir_buffer[i - 100], un_prev_data);

        // 滑动平均滤波
        max30102_data.red_buffer[i - 100] = SmoothData(max30102_data.red_buffer[i - 100], hr_buffer, &hr_index);
        max30102_data.ir_buffer[i - 100] = SmoothData(max30102_data.ir_buffer[i - 100], spo2_buffer, &spo2_index);

        // 更新最小值和最大值
        if (max30102_data.min_value > max30102_data.red_buffer[i - 100])
            max30102_data.min_value = max30102_data.red_buffer[i - 100];
        if (max30102_data.max_value < max30102_data.red_buffer[i - 100])
            max30102_data.max_value = max30102_data.red_buffer[i - 100];
    }
    // printf("aaa\r\n");
}
// 数据处理与显示
void Process_And_Display_Data(void)
{
    if (max30102_data.heart_rate_valid == 1 && max30102_data.heart_rate < 100)
    {
        sensor.heart_rate = max30102_data.heart_rate;
        sensor.spO2 = max30102_data.spO2;
        printf("dis_hr:%d  ,dis_spo2:%d\r\n", sensor.heart_rate, sensor.spO2);
    }
}
uint8_t spo2_cnt = 0;
uint8_t heart_cnt = 0;
void max30102_abnormal_event(void)
{
    static uint8_t cnt = 0;
    // printf("max30102_data.prev_data:%d\r\n",max30102_data.ir_buffer[cnt] );
    if (max30102_data.ir_buffer[cnt] < WEAR_THRESHOLD)
    {
        cnt++;
        if(cnt >= 2){
        event.wear_flag = DETECTED;
        sensor.heart_rate = 0;
        sensor.spO2 = 0;
        printf("not wear \r\n");
        cnt = 0;
        }
    }
    else
    {
        event.wear_flag = NOT_DETECTED;
									    // 数据处理与显示
				Process_And_Display_Data();
        if (sensor.heart_rate < 60 || sensor.heart_rate > 100)
        {
						heart_cnt ++;
					if(heart_cnt > 5){
						            event.heart_flag = DETECTED;
            printf("not heart_rate\r\n");
					}
        }
        else
        {
						heart_cnt = 0;
            event.heart_flag = NOT_DETECTED;
        }
        if (sensor.spO2 < 95 || sensor.spO2 > 100)
        {
					spo2_cnt ++;
					if(spo2_cnt >= 5){
						            event.spo2_flag = DETECTED;
            printf("spo2 not \r\n");
					}

        }
        else
        {
					spo2_cnt = 0;
            event.heart_flag = NOT_DETECTED;
        }
    }
}
void max30102_task(void)
{
    // 从 MAX30102 读取数据，并进行心率补偿
    MAX30102_Read_Data();
    // 计算心率和血氧值
    Calculate_Heart_Rate_and_SpO2();
    // 更新信号的最小值和最大值，并应用滤波
    Update_Signal_Min_Max();

    // 检测逻辑
    max30102_abnormal_event();
    //  printf("wear flag:%d\r\n", wear_flag);
}
