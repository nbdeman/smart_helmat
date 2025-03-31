#include "led_app.h"
uint8_t ucled[2] = {1, 1}; // 定义LED状态数组，初始状态为第一个LED亮，第二个LED

/**
 * @brief 显示LED状态
 *
 * @param ucLed 指向LED状态数组的指针
 */
void led_disp(uint8_t *ucLed)
{
    // 定义一个临时变量，用于存储当前LED状态
    uint8_t temp = 0xFC; // 初始值为0xFC，表示两个LED的默认关闭状态
    // 定义一个静态变量，用于存储上一次的LED状态
    static uint8_t temp_old = 0xff; // 初始值为0xFF，表示所有LED关闭

    // 遍历LED状态数组，更新temp变量
    for (int i = 0; i < 2; i++)
    {
        // 根据ucled数组的值更新temp变量的对应位
        temp |= (ucled[i] << (i));
    }
    // 如果当前状态与上一次状态不同，则更新LED硬件状态
    if (temp != temp_old)
    {
        // 清除PF9和PF10的输出状态
        GPIOF->ODR &= ~(1 << 9);  // 清除PF9
        GPIOF->ODR &= ~(1 << 10); //  清除PF10

        // 根据temp变量的值设置PF9和PF10的输出状态
        if (temp & 0x01) // 如果temp的第0位为1，则点亮PF9
        {
            GPIOF->ODR |= (1 << 9);
        }
        if (temp & 0x02) // 如果temp的第1位为1，则点亮PF10
        {
            GPIOF->ODR |= (1 << 10);
        }

        // 更新静态变量temp_old为当前状态
        temp_old = temp;
    }
}

/**
 * @brief LED显示运行
 *
 *
 */
void led_proc(void)
{
    // 执行led显示函数
    led_disp(ucled);
    // printf("aaaaaaaaaaaaaaaaaaaaaaa\r\n");
}
