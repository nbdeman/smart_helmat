#include "ringbuffer.h"
#if UART == ENABLE_APP
// 初始化环形缓冲区
void ringbuffer_init(ringbuffer_t *rb)
{
    // 将读指针和写指针的初始值设置为0
    rb->r = 0;
    rb->w = 0;
    // 初始化缓冲区内容，清零整个缓冲区
    memset(rb->buffer, 0, sizeof(uint8_t) * RINGBUFFER_SIZE);
    // 初始化缓冲区中的项目数量为0
    rb->itemCount = 0;
}

// 检查环形缓冲区是否已满
uint8_t ringbuffer_is_full(ringbuffer_t *rb)
{
    // 如果缓冲区中的项目数量等于缓冲区大小，则返回1（已满），否则返回0（未满）
    return (rb->itemCount == RINGBUFFER_SIZE);
}

// 检查环形缓冲区是否为空
uint8_t ringbuffer_is_empty(ringbuffer_t *rb)
{
    // 如果缓冲区中的项目数量为0，则返回1（为空），否则返回0（不为空）
    return (rb->itemCount == 0);
}

// 向环形缓冲区写入数据
int8_t ringbuffer_write(ringbuffer_t *rb, uint8_t *data, uint32_t num)
{
    // 如果缓冲区已满，返回-1表示写入失败
    if (ringbuffer_is_full(rb))
        return -1;

    // 循环将数据写入缓冲区
    while (num--)
    {
        rb->buffer[rb->w] = *data++;           // 将数据写入缓冲区，并移动写指针
        rb->w = (rb->w + 1) % RINGBUFFER_SIZE; // 写指针循环移动
        rb->itemCount++;                       // 增加缓冲区中的项目数量
    }

    return 0; // 写入成功返回0
}

// 从环形缓冲区读取数据
int8_t ringbuffer_read(ringbuffer_t *rb, uint8_t *data, uint32_t num)
{
    // 如果缓冲区为空，返回-1表示读取失败
    if (ringbuffer_is_empty(rb))
        return -1;

    // 循环从缓冲区读取数据
    while (num--)
    {
        *data++ = rb->buffer[rb->r];           // 从缓冲区读取数据，并移动读指针
        rb->r = (rb->r + 1) % RINGBUFFER_SIZE; // 读指针循环移动
        rb->itemCount--;                       // 减少缓冲区中的项目数量
    }

    return 0; // 读取成功返回0
}
#endif
