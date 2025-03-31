#include "dht11.h"

// 配置GPIOB引脚0为输入模式
void DHT11_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 不使用上拉或下拉电阻
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// 配置GPIOB引脚0为输出模式
void DHT11_IO_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 不使用上拉或下拉电阻
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // 低速
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void DHT11_DQ_OUT(GPIO_PinState state)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, state);
}
GPIO_PinState DHT11_DQ_IN()
{
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
}

// 复位DHT11
void DHT11_Rst(void)
{
    DHT11_IO_OUT();               // SET OUTPUT
    DHT11_DQ_OUT(GPIO_PIN_RESET); // 拉低DQ
    HAL_Delay(20);                // 拉低至少18ms
    DHT11_DQ_OUT(GPIO_PIN_SET);   // DQ=1
    delay_us(30);                 // 主机拉高20~40us
}

// 等待DHT11的回应
// 返回1:未检测到DHT11的存在
// 返回0:存在
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    DHT11_IO_IN();                       // SET INPUT
    while (DHT11_DQ_IN() && retry < 100) // DHT11会拉低40~80us
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 100)
        return 1;
    else
        retry = 0;
    while (!DHT11_DQ_IN() && retry < 100) // DHT11拉低后会再次拉高40~80us
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 100)
        return 1;
    return 0;
}

// 从DHT11读取一个位
// 返回值：1/0
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    while (DHT11_DQ_IN() && retry < 100) // 等待变为低电平
    {
        retry++;
        delay_us(1);
    }
    retry = 0;
    while (!DHT11_DQ_IN() && retry < 100) // 等待变高电平
    {
        retry++;
        delay_us(1);
    }
    delay_us(40); // 等待40us
    if (DHT11_DQ_IN())
        return 1;
    else
        return 0;
}

// 从DHT11读取一个字节
// 返回值：读到的数据
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i, dat;
    dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

// 从DHT11读取一次数据
// temp:温度值(范围:0~50°)
// humi:湿度值(范围:20%~90%)
// 返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(Sensor_data *data)
{
    uint8_t buf[5];
    uint8_t i;
    DHT11_Rst();
    if (DHT11_Check() == 0)
    {
        for (i = 0; i < 5; i++) // 读取40位数据
        {
            buf[i] = DHT11_Read_Byte();
        }
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            data->humi = buf[0];
            data->temp = ((buf[2] * 10.0f) + (float)buf[3]) / 10.0f;
        }
    }
    else
        return 1;
    return 0;
}

// 初始化DHT11的IO口 DQ 同时检测DHT11的存在
// 返回1:不存在
// 返回0:存在
uint8_t DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // PB0 输出高
    DHT11_Rst();                                        // 复位DHT11
    return DHT11_Check();                               // 等待DHT11的回应
}

void dht11_proc(void)
{
    DHT11_Read_Data(&sensor);
   //printf("temp:%.2f,humi:%d\r\n", sensor.temp, sensor.humi);
}
