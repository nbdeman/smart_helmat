#ifndef _DHT11_H
#define _DHT11_H

#include "bsp_system.h"
uint8_t DHT11_Init(void);                   // dht11初始化

void dht11_proc(void);

#endif
