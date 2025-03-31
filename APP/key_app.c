#include "key_app.h" // 包含按键应用头文件
/* 注释说明：
1. 本代码采用ANSI编码格式，适用于Keil开发环境
2. 使用状态机实现按键检测，支持单击/双击/长按识别
3. 消抖机制采用3次采样法，确保信号稳定
4. 时间参数说明：
   - 50ms：长按判定阈值
   - 30ms：按下稳定时间
   - 15ms：释放稳定时间
5. LED控制通过修改ucled数组实现，需与硬件驱动配合使用 */

/* 按键状态机结构体定义 */
typedef struct
{
  GPIO_TypeDef *gpiox; // GPIO端口定义，例如GPIOA/GPIOB等
  uint16_t pin;        // GPIO引脚编号，例如GPIO_PIN_0/GPIO_PIN_1
  uint16_t ticks;      // 计时计数器（单位：ms）
  uint8_t level;       // 当前有效电平（0：低电平有效，1：高电平有效）
  uint8_t id;          // 按键ID标识符
  uint8_t state;       // 按键状态（0：空闲，1：按下检测，2：释放检测）
  uint8_t debouce_cnt; // 消抖计数器
  uint8_t repeat;      // 连击计数器（用于长按检测）
} button;

button btns[3]; // 按键对象数组，支持3个独立按键

void key_init(void)
{

  btns[0].gpiox = GPIOE;
  btns[0].pin = GPIO_PIN_4;
  btns[0].level = 1;
  btns[0].id = 0;

  btns[1].gpiox = GPIOE;
  btns[1].pin = GPIO_PIN_3;
  btns[1].level = 1;
  btns[1].id = 1;

  btns[2].gpiox = GPIOE;
  btns[2].pin = GPIO_PIN_2;
  btns[2].level = 1;
  btns[2].id = 2;
}

/* 按键状态机处理函数 */
void key_task(button *btn)
{
  // 读取当前GPIO电平状态
  uint8_t gpio_level = HAL_GPIO_ReadPin(btn->gpiox, btn->pin);

  // 当按键处于激活状态时（state > 0），进行时间累计
  if (btn->state > 0)
    btn->ticks++;

  /* 消抖处理机制（3次采样稳定） */
  if (btn->level != gpio_level)
  {
    // 连续3次检测到电平变化则更新状态
    if (++(btn->debouce_cnt) >= 3)
    {
      btn->level = gpio_level; // 更新有效电平状态
      btn->debouce_cnt = 0;    // 重置消抖计数器
    }
  }
  else
  {
    btn->debouce_cnt = 0; // 电平稳定时重置计数器
  }

  /* 状态机处理 */
  switch (btn->state)
  {
  case 0:                // 空闲状态
    if (btn->level == 0) // 检测到有效电平（按下）
    {
      btn->ticks = 0;  // 重置计时器
      btn->repeat = 1; // 初始化连击计数器
      btn->state = 1;  // 进入按下检测状态
    }
    break;
  case 1:                // 按下检测状态
    if (btn->level != 0) // 检测到电平释放
    {
      if (btn->ticks >= 50) // 长按超过50ms处理
      {
        ucled[1] ^= 1;  // 切换LED1状态
        ucled[0] ^= 1;  // 同步切换LED0
        btn->state = 0; // 返回空闲状态
      }
      else
      {
        btn->ticks = 0; // 重置计时器
        btn->state = 2; // 进入释放检测状态
      }
    }
    else if (btn->ticks >= 30) // 持续按下超过30ms
    {
      btn->repeat = 0; // 标记为长按状态
    }
    break;
  case 2:                 // 释放检测状态
    if (btn->ticks >= 15) // 释放后等待15ms处理
    {
      btn->state = 0;       // 返回空闲状态
      if (btn->repeat == 1) // 单次点击
      {
        ucled[0] ^= 1; // 切换LED0
      }
      else if (btn->repeat == 2) // 双击操作
      {
        ucled[1] ^= 1; // 切换LED
      }
    }
    else
    {
      if (btn->level == 0) // 再次检测到按下
      {
        btn->repeat++;  // 增加连击计数
        btn->ticks = 0; // 重置计时器
        btn->state = 1; // 返回按下检测状态
      }
    }
    break;
  }
}

/* 按键轮询处理函数 */
void key_proc(void)
{
  for (uint8_t i = 0; i < 3; i++) // 遍历所有按键
  {
    key_task(&btns[i]); // 处理每个按键的状态
  }
}
