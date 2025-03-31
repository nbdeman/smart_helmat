#include "esp_8266.h"

/**
 * @简要  	onenet产品的属性结构体
 * @注意	这里需要按照onenet产品中创建的属性值来创建并初始化OneNET_MQTT_Data结构体内容，数据类型和名称要一致
 */
OneNET_MQTT_Data temperature = {
    "value",
    TYPE_FLOAT,
    {.float_value = 0.0f},
    -20.0f,
    60.0f,
}; // 温度
OneNET_MQTT_Data humidity = {
    "value_humi",
    TYPE_INT,
    {.int_value = 0},
    5,
    95,

}; // 湿度
OneNET_MQTT_Data PPM = {
    "v_value",
    TYPE_FLOAT,
    {.float_value = 0.0f},
    0,
    65535.0f,
}; // PPM浓度
OneNET_MQTT_Data longitude = {
    "longitude",
    TYPE_FLOAT,
    {.float_value = 0.0f},
    -180.0f,
    180.0f,
}; // 经度
OneNET_MQTT_Data latitude = {
    "latitude",
    TYPE_FLOAT,
    {.float_value = 0.0f},
    -90.0f,
    90.0f,
}; // 纬度
OneNET_MQTT_Data heart_rate = {
    "heart_rate",
    TYPE_INT,
    {.int_value = 0},
    30,
    200,
}; // 心率
OneNET_MQTT_Data spo2 = {
    "spo2",
    TYPE_INT,
    {.int_value = 0.0f},
    50,
    100,
}; // 血氧浓度
OneNET_MQTT_Data fall = {
    "fall",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 跌倒检测
OneNET_MQTT_Data sos = {
    "sos",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // sos报警
OneNET_MQTT_Data MQ2_Beyond = {
    "MQ2_beyond",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 可燃气体浓度超标检测
OneNET_MQTT_Data wear = {
    "wear_flag",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 脱帽检测
OneNET_MQTT_Data spo2_flag = {
    "spo2_flag",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 血氧浓度检测
OneNET_MQTT_Data heart_flag = {
    "heart_flag",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 心率超标检测
OneNET_MQTT_Data collsion = {
    "collsion_flag",
    TYPE_BOOL,
    {.bool_value = 0},
    0,
    1,
}; // 心率超标检测
/**
 * @简要  	onenet产品的属性值指针数组和数组大小
 * @注意	这里只适用数据上报函数，填写需要上报数据的属性结构体指针
 */
static OneNET_MQTT_Data *onenet_data_array[] = {
    &temperature,
    &humidity,
    &PPM,
    &spo2,
    &heart_rate,
    &longitude,
    &latitude,
    &fall,
    &wear,
    &sos,
    &spo2_flag,
    &collsion,
    &heart_flag,
    &MQ2_Beyond,
};
#define DATA_ARRAY_SIZE (sizeof(onenet_data_array) / sizeof(onenet_data_array[0]))

/**
 * @简要  	替换外部函数名字
 * @注意	这里后面的函数名字需要替换你自己的函数名字
 */
#define esp8266_delay_ms osDelay
#define esp8266_send_data(value) HAL_UART_Transmit(&huart2, (uint8_t *)value, strlen((char *)value), HAL_MAX_DELAY);
#define esp8266_log_print printf
#define W_RxBuffer uart2_dma_buffer
/**
 * @简要  	无线模块状态结构体
 * @注意	无
 */
ESP8266_TypeDef ESP8266_Status = {0, 0};

/*数据及标志位清除*/
/**
 * @简要  清除无线模块缓存大小
 * @参数  无
 * @注意	需要在本次接收数据后，不再适用数据后，再清除缓存
 * @返回值 无
 */
void esp8266_clear_buffer(void)
{
    memset(W_RxBuffer, 0, sizeof(W_RxBuffer));
    // ESP_STA = 0;
}
/**
  * @简要  判断MQTT服务器连接情况
  * @参数  	无
  * @注意	连接或者断开mqtt服务器时，无线模块的串口会收到下面数据
  * @数据示例  	1.mqtt恢复连接：+MQTTCONNECTED:0,1,"mqtts.heclouds.com","1883","",1
        2.mqtt断开连接：+MQTTDISCONNECTED:0
  * @返回值 mqtt服务器连接情况
  */
uint8_t esp8266_mqtt_connect_status(void)
{
    if (strstr((char *)W_RxBuffer, "+MQTTDISCONNECTED:0") != NULL)
        return !W_OK;
    if (strstr((char *)W_RxBuffer, "+MQTTCONNECTED:0,1,\"mqtts.heclouds.com\",\"1883\",\"\",1") != NULL)
        return W_OK;
    return 0xFF;
}
/**
  * @简要  判断wifi连接情况
  * @参数  	无
  * @注意	连接或者断开wifi时，无线模块的串口会收到下面数据
  * @数据示例  	1.wifi恢复连接：WIFI GOT IP
        2.wifi断开连接：WIFI DISCONNECT
  * @返回值 wifi连接情况
  */
uint8_t esp8266_wifi_connect_status(void)
{
    if (strstr((char *)W_RxBuffer, "WIFI DISCONNECT") != NULL)
        return !W_OK;
    if (strstr((char *)W_RxBuffer, "WIFI GOT IP") != NULL)
        return W_OK;
    return 0xFF;
}
/**
 * @简要  获取无线模块数据接收标志位
 * @参数  	无
 * @注意	无线模块串口接收到具有换行回车的数据后，会设置该标志位
 * @返回值 1-接收 	0-未接收
 */
uint8_t esp8266_get_receive_flag(void)
{
    uint8_t flag = ESP8266_Status.receiveDataFlag;
    ESP8266_Status.receiveDataFlag = 0;
    return flag;
}

/**
  * @简要  	无线模块串口发送AT指令
  * @参数  	char *cmd：要发送的指令字符串
      char *res：期望的返回值字符串结果
      uint8_t sendCount：最大发送次数
      uint8_t clear_buffer 是否清除接收缓存
      uint16_t delay_xms：每次发送AT指令后x毫秒后判断结果
      uint8_t printf_enable:选择是否打印本次指令
  * @注意	根据每个AT指令的响应时间或者具有多个回复的AT指令，去填写适当的delay_xms延时，例如连接wifi的AT指令，需要大约2000ms以及会回复3个AT数据，可以填写2000ms延时等待
  * @返回值 0：不符合预期接收数据
      1：符合预期接收数据
  */
uint8_t esp8266_send_command(char *cmd, char *res, uint8_t sendCount, uint8_t clear_buffer, uint16_t delay_xms, uint8_t printf_enable)
{
    if (printf_enable == W_ENABLE)
        esp8266_log_print("%s", cmd); // 这里可以打印每次发送的指令
    while (sendCount--)
    {

        esp8266_send_data(cmd);                 // AT指令发送
        esp8266_delay_ms(delay_xms);            // 适当增加点延迟，等待串口接收完成
        if (esp8266_get_receive_flag() == W_OK) // 如果串口接收到换行回车为结尾的数据
        {
#if ONENET_MQTT_SET_ENABLE
            if (esp8266_get_onenet_command_flag() == W_OK)
            {
                ESP8266_Status.receiveDataFlag = 1;
                return 0;
            } // 这里特殊判断是否有属性设置信息，优先级最高，直接中断当前AT指令
#endif
            if (strstr((const char *)W_RxBuffer, res) != NULL) // 若找到关键字
            {
                if (clear_buffer == W_TRUE)
                    esp8266_clear_buffer(); // 清除数组
                return 1;                   // 退出，返回0-成功
            }
            esp8266_log_print("error recovery data:%s", W_RxBuffer); // 当接收错误时，打印出接收的数据，方便调试
            if (clear_buffer == W_TRUE)
                esp8266_clear_buffer();
        }
        esp8266_delay_ms(500);
    }
    return 0; // 返回1-失败
}
/**
 * @简要  无线模块初始化错误处理函数
 * @参数  ErrorCode: 错误码，对应的错误状态
 * @返回值 无
 */
void esp8266_error_handler(uint16_t error_code)
{
    esp8266_log_print("\r\n	Here are the AT instructions that did not meet expectations		\r\n\r\n");
    if (error_code & (0x01 << 0))
        esp8266_log_print("1.AT+RST\r\n\r\n");
    if (error_code & (0x01 << 1))
        esp8266_log_print("2.ATE0\r\n\r\n");
    if (error_code & (0x01 << 2))
        esp8266_log_print("3.AT+CWMODE=1\r\n\r\n");
    if (error_code & (0x01 << 3))
        esp8266_log_print("4.AT+CWDHCP=1,1\r\n\r\n");
    if (error_code & (0x01 << 4))
        esp8266_log_print("5.%s\r\n", WIRELESS_WIFI_INFO);
    if (error_code & (0x01 << 5))
        esp8266_log_print("6.%s\r\n", ONENET_MQTT_USERCFG_INFO);
    if (error_code & (0x01 << 6))
        esp8266_log_print("7.%s\r\n", ONENET_MQTT_SERVER_INFO);
    if (error_code & (0x01 << 7))
        esp8266_log_print("8.%s\r\n", ONENET_MQTT_REPLY_TOPIC);
#if ONENET_MQTT_SET_ENABLE
    if (error_code & (0x01 << 8))
        esp8266_log_print("9.%s\r\n", ONENET_MQTT_SET_TOPIC);
#endif
    esp8266_log_print("\r\n	End		\r\n");
}

/**
 * @简要  无线模块初始化
 * @参数  	无
 * @注意	如果初始化效果不理想，或者响应错误，请检查AT指令是否正确，并可以适当加大AT指令的响应时间或者发送次数
 * @返回值 无
 */
void esp8266_init(void)
{
    esp8266_log_print("Start MQTT service\r\n");
    // 复位模块，清除所有状态
    if (esp8266_send_command("AT+RST\r\n", "", 3, W_TRUE, 500, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 0);
    // 关闭AT指令回显，减少进入串口中断的“\r\n”判断
    if (esp8266_send_command("ATE0\r\n", "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 1);
    // 设置模式一，配置它作为Wi-Fi客户端，让其可以连接到无线路由器
    if (esp8266_send_command("AT+CWMODE=1\r\n", "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 2);
    // 开启DHCP,在连接到Wi-Fi网络时能够自动获得IP地址
    if (esp8266_send_command("AT+CWDHCP=1,1\r\n", "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 3);
    // 连接2.4GHz wifi
    if (esp8266_send_command(WIRELESS_WIFI_INFO, "GOT IP", 3, W_TRUE, 2000, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 4) | ERROR_WiFi_CONNECT;
    // 用于配置MQTT客户端的用户参数
    if (esp8266_send_command(ONENET_MQTT_USERCFG_INFO, "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 5);
    // 连接onenet的MQTT服务器
    if (esp8266_send_command(ONENET_MQTT_SERVER_INFO, "OK", 3, W_TRUE, 500, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 6) | ERROR_MQTT_CONNECT;
    // 订阅设备属性上报响应主题
    if (esp8266_send_command(ONENET_MQTT_REPLY_TOPIC, "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 7);
#if ONENET_MQTT_SET_ENABLE
    // 订阅设备属性设置请求
    if (esp8266_send_command(ONENET_MQTT_SET_TOPIC, "OK", 3, W_TRUE, 50, W_ENABLE) != W_OK)
        ESP8266_Status.error_code |= (1 << 8);
#endif
    if (ESP8266_Status.error_code == 0) // 如果所有AT指令都没有错误
    {
        esp8266_log_print("\r\n	!!!!!	MQTT service started successfully	!!!!!	\r\n\r\n");
    }
    else // 如果有错误
    {
        esp8266_log_print("\r\n	*****	MQTT service failed to start	******	\r\n\r\n");
        esp8266_error_handler(ESP8266_Status.error_code); // 错误处理函数
    }
}
/**
 * @简要  无线模块系统事件处理函数
 * @参数  	无
 * @注意	无
 * @返回值 无
 */
void esp8266_system_handler(void)
{
    if (ESP8266_Status.error_code & ERROR_WiFi_CONNECT)
        esp8266_log_print("ERROR_WiFi_CONNECT:	WIFI DISCONNECT\r\n");
    if (ESP8266_Status.error_code & ERROR_MQTT_CONNECT)
        esp8266_log_print("ERROR_MQTT_CONNECT:	MQTT DISCONNECT\r\n");
    if (ESP8266_Status.error_code & ERROR_PUBLISH)
        esp8266_log_print("ERROR_PUBLISH\r\n");
}
/**
 * @简要  无线模块onenet云平台数据处理
 * @参数  	无
 * @注意	这里统一修改有关onenet平台的属性值，然后统一上报数据
 * @返回值 无
 */
void esp8266_onenet_data_handler(void)
{

    PPM.value.float_value = sensor.c3h8_ppm;        // 气体浓度上报
    heart_rate.value.int_value = sensor.heart_rate; // 心率值
    spo2.value.int_value = sensor.spO2;             // 血氧值

    temperature.value.float_value = sensor.temp;    // 赋值给onenet对应的属性值，注意属性的数据类型
    humidity.value.int_value = sensor.humi;         // 赋值给onenet对应的属性值，注意属性的数据类型
    longitude.value.float_value = sensor.longitude; // 经纬度信息上报
    latitude.value.float_value = sensor.latitude;

    sos.value.bool_value = event.sos;
    spo2_flag.value.bool_value = event.spo2_flag;
    heart_flag.value.bool_value = event.heart_flag;
    fall.value.bool_value = event.fall_flag;
    collsion.value.bool_value = event.collision_flag;
    MQ2_Beyond.value.bool_value = event.ppm_flag;
    wear.value.bool_value = event.wear_flag;
}
/**
  * @简要  数据上报至onenet云平台
  * @参数  	无
  * @注意	根据OneNET_MQTT_Data创建的属性结构体，并且使用指针数组封装，然后统一处理后上报到onenet云平台
      这里会发送两个AT指令数据，第一个是mqtt长指令上报AT指令，发送后进入长数据模式
      第二个是发送json格式的数据上报至onenet云平台
      需要注意第二个AT指令组合后的长度是否超出数组缓存大小
  * @示例：	第一个AT指令:AT+MQTTPUBRAW=0,"$sys/HbJo2787Fn/device-001/thing/property/post",182,0,0
      第二个AT指令：{"id":"123","params":{"ChuangLian_Control":{"value":false},"JiaShiQi_Control":{"value":false},"sensor_status":{"value":0},"temperature":{"value":13.5},"humidity":{"value":39.0}}}
  * @返回值 无
  */
void esp8266_publish_data(void)
{
#define BUFFER_SIZE 512
#define AT_COMMAND_SIZE 256
    uint16_t bufferPos = 0;
    uint16_t atCommandPos = 0;
    char globalBuffer[BUFFER_SIZE];
    char atCommand[AT_COMMAND_SIZE];
    static uint8_t error_send_count = 0;

    bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos, "{\"id\":\"123\",\"params\":{"); // 手动拼接JSON数据
    // 遍历指针数组，根据类型拼接数据
    for (unsigned char i = 0; i < DATA_ARRAY_SIZE; ++i)
    {
        switch (onenet_data_array[i]->type)
        {
        case TYPE_BOOL:
            bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos,
                                  "\"%s\":{\"value\":%s}%s",
                                  onenet_data_array[i]->name, onenet_data_array[i]->value.bool_value ? "true" : "false",
                                  (i < DATA_ARRAY_SIZE - 1) ? "," : "");
            break;
        case TYPE_INT:
            if (onenet_data_array[i]->value.int_value > onenet_data_array[i]->max_value || onenet_data_array[i]->value.int_value < onenet_data_array[i]->min_value)
                break;
            bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos,
                                  "\"%s\":{\"value\":%d}%s",
                                  onenet_data_array[i]->name, onenet_data_array[i]->value.int_value,
                                  (i < DATA_ARRAY_SIZE - 1) ? "," : "");
            break;
        case TYPE_FLOAT:
            if (onenet_data_array[i]->value.float_value > onenet_data_array[i]->max_value || onenet_data_array[i]->value.float_value < onenet_data_array[i]->min_value)
                break;
            bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos,
                                  "\"%s\":{\"value\":%.1f}%s",
                                  onenet_data_array[i]->name, onenet_data_array[i]->value.float_value,
                                  (i < DATA_ARRAY_SIZE - 1) ? "," : "");
            break;
        case TYPE_STRING:
            bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos,
                                  "\"%s\":{\"value\":\"%s\"}%s",
                                  onenet_data_array[i]->name, onenet_data_array[i]->value.string_value,
                                  (i < DATA_ARRAY_SIZE - 1) ? "," : "");
            break;
        }
    }

    // 拼接JSON结尾
    bufferPos += snprintf(globalBuffer + bufferPos, BUFFER_SIZE - bufferPos, "}}\r\n\r\n");

    // 如果上报数据的字符数组大于缓存，打印报错
    if (bufferPos >= BUFFER_SIZE)
    {
        // 处理错误，例如通过日志记录
        printf("\r\nERROR:publish_data buffer overflow\r\n");
        return;
    }

    // 拼接 AT 命令
    atCommandPos += snprintf(atCommand + atCommandPos, AT_COMMAND_SIZE - atCommandPos,
                             "%s,%d,0,0\r\n", ONENET_MQTT_PUBTOPIC, (int)bufferPos);

    // 检查 AT 命令是否溢出
    if (atCommandPos >= AT_COMMAND_SIZE)
    {
        printf("\r\nERROR:atCommand buffer overflow\r\n");
        return;
    }
    esp8266_send_command(atCommand, "OK", 1, W_TRUE, 25, W_ENABLE); // 发送 AT 命令进入上报长数据模式
    // 发送数据
    if (esp8266_send_command(globalBuffer, "{\"id\":\"123\",\"code\":200,\"msg\":\"success\"}", 1, W_TRUE, 300, W_ENABLE) != W_OK)
        error_send_count++;
    else
        error_send_count = 0, ESP8266_Status.error_code &= ~ERROR_PUBLISH;

    if (error_send_count >= 5)
        ESP8266_Status.error_code |= ERROR_PUBLISH; // 如果连续失败次数达到 5 次，设置错误代码
}

#if ONENET_MQTT_SET_ENABLE

/**
 * @简要  判断接收的数据是否是属性设置消息
 * @参数  	无
 * @注意	只需要判断订阅消息内容：$sys/HbJo2787Fn/device-001/thing/property/set
 * @数据示例： +MQTTSUBRECV:0,"$sys/HbJo2787Fn/device-001/thing/property/set",63,{"id":"4","version":"1.0","params":{"ChuangLian_Control":true}}
 * @返回值 是否是属性设置消息
 */
uint8_t esp8266_get_onenet_command_flag(void)
{
    // 判断如果是属性设置数据包
    if (strstr((char *)W_RxBuffer, ONENET_MQTT_REVEIVE_SET_TOPIC) != NULL)
        return W_OK;

    return !W_OK;
}

/**
 * @简要  属性设置接收信息-提取id号
 * @参数  	char *jsonData：json数据
 * @注意	传入json格式的数据，需要将原始接收的数据分割‘{’前面的内容
 * @参数示例：{"id":"4","version":"1.0","params":{"ChuangLian_Control":true}}
 * @返回值 当前属性设置信息的id号
 */
uint32_t esp8266_get_command_id(char *jsonData)
{
    uint32_t cmd_id = 0; // 定义一个id整形变量

    cJSON *json_obj = cJSON_Parse(jsonData); // 判断JSON字符串是否合法
    if (json_obj == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            esp8266_log_print("Error before: %s\n", error_ptr);
        }
        return 0xFFFFFFFF;
    }

    cJSON *id_item = cJSON_GetObjectItem(json_obj, "id"); // 提取id字段
    if (cJSON_IsString(id_item) && (id_item->valuestring != NULL))
    {
        cmd_id = atoi(id_item->valuestring); // 将id字符串转为整形
    }

    cJSON_Delete(json_obj); // 清理JSON对象

    return cmd_id;
}

/**
 * @简要  根据属性设置信息-提取命令内容
 * @参数  	char *jsonData：json数据
 * @注意	需要根据对应的数据类型执行对应的解析函数，cJSON_IsBool(cmd_item)或者cJSON_IsNumbmer(cmd_item)
 * @示例  {"id":"4","version":"1.0","params":{"ChuangLian_Control":true}}
 * @返回值 无
 */
void esp8266_receiver_command_extract(char *jsonData)
{
    // 检测字符串是否符合json格式
    cJSON *json_obj = cJSON_Parse(jsonData);
    if (json_obj == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            esp8266_log_print("Error before: %s\r\n", error_ptr);
        }
        return;
    }

    // 提取params字段
    cJSON *params_item = cJSON_GetObjectItem(json_obj, "params");
    if (params_item == NULL)
    {
        esp8266_log_print("Params not found\r\n");
        cJSON_Delete(json_obj);
        return;
    }

    cJSON *cmd_item = NULL;

    cmd_item = cJSON_GetObjectItem(params_item, ChuangLian_Control.name);
    if (cmd_item != NULL && cJSON_IsBool(cmd_item))
    {
        ChuangLian_Control.value.bool_value = (uint8_t)cmd_item->valueint;
        esp8266_log_print("ChuangLian_Control.value:%d\r\n", ChuangLian_Control.value.bool_value);
    }

    cmd_item = cJSON_GetObjectItem(params_item, JiaShiQi_Control.name);
    if (cmd_item != NULL && cJSON_IsBool(cmd_item))
    {
        JiaShiQi_Control.value.bool_value = (uint8_t)cmd_item->valueint;
        esp8266_log_print("JiaShiQi_Control.value:%u\r\n", JiaShiQi_Control.value.bool_value);
    }

    cmd_item = cJSON_GetObjectItem(params_item, DiaoDeng_Control.name);
    if (cmd_item != NULL && cJSON_IsNumber(cmd_item))
    {
        DiaoDeng_Control.value.int_value = (uint8_t)cmd_item->valueint;
        esp8266_log_print("DiaoDeng_Control.value:%u\r\n", DiaoDeng_Control.value.int_value);
    }

    cJSON_Delete(json_obj); // 释放内存
}

/**
  * @简要  接收到属性设置数据后回应
  * @参数  	uint32_t respond_id：回应的id
      uint16_t code：回应的代码，默认：200
      char *msg：回应的字符，默认：success
  * @注意	主要用于APP或者微信小程序中调用API执行HTTP操作时，需要返回执行状态的场景
  * @返回值 无
  */
void esp8266_receive_command_respond(uint32_t respond_id, uint16_t code, char *msg)
{
    char jsonData[256] = {0};
    snprintf(jsonData, sizeof(jsonData),
             "%s,\"{\\\"id\\\":\\\"%d\\\"\\,\\\"code\\\":%d\\,\\\"msg\\\":\\\"%s\\\"}\",0,0\r\n",
             ONENET_MQTT_PUB_SET, respond_id, code, msg);
    esp8266_send_command(jsonData, "OK", 1, W_TRUE, 50, W_ENABLE);
}

/**
 * @简要  无线模块命令控制
 * @参数  	无
 * @注意	这里会在接收到属性设置信息后执行函数，根据提取后的指令，执行不同的操作，最后需要修改对应的数据值
 * @返回值 无
 */
void esp8266_command_control(void)
{
    if (ChuangLian_Control.value.bool_value == W_TRUE)
        ChuangLian_Control_ON();
    else if (ChuangLian_Control.value.bool_value == W_FALSE)
        ChuangLian_Control_OFF();

    if (JiaShiQi_Control.value.bool_value == W_TRUE)
        JiaShiQi_Control_ON();
    else if (JiaShiQi_Control.value.bool_value == W_FALSE)
        JiaShiQi_Control_OFF();

    if (DiaoDeng_Control.value.int_value >= 0 && DiaoDeng_Control.value.int_value <= 100)
        DiaoDeng_Control_PWM(DiaoDeng_Control.value.int_value);
}

/**
  * @简要  无线模块处理接收到的属性设置数据
  * @参数  	无
  * @注意	将数据转为json格式，然后提取id号和指令，最后响应属性设置
      主要用于APP或者微信小程序中调用API执行HTTP操作时，需要返回执行状态的场景
  * @示例：	原始数据： +MQTTSUBRECV:0,"$sys/HbJo2787Fn/device-001/thing/property/set",63,{"id":"4","version":"1.0","params":{"ChuangLian_Control":true}}
      转为json格式：{"id":"4","version":"1.0","params":{"ChuangLian_Control":true}}
      提取id号转为整形：4
      提取参数params： "ChuangLian_Control":true
  * @返回值 无
  */
void esp8266_processing_command(void)
{
    char *jsonData = NULL;
    uint32_t cmd_id;
    jsonData = strstr((const char *)W_RxBuffer, ONENET_MQTT_REVEIVE_SET_TOPIC); // 先截取出属性设置部分的字符串
    if (jsonData == NULL)
        strchr((const char *)W_RxBuffer, '{'); // 提取json数据
    else
        jsonData = strchr((const char *)jsonData, '{');      // 提取json数据
    esp8266_log_print("jsonData:\r\n%s\r\n", jsonData);      // 显示接收到的json数据
    cmd_id = esp8266_get_command_id(jsonData);               // 获得命令的id号
    esp8266_receiver_command_extract(jsonData);              // 提取指令
    esp8266_receive_command_respond(cmd_id, 200, "success"); // 回应onenet
}

#endif

/**
 * @简要  无线模块串口接收数据处理函数
 * @参数  	无
 * @注意	在接收到数据后，判断是什么内容，根据对应内容执行对应的操作，最后清除接收数据缓存
 * @返回值 无
 */
void esp8266_receive_data_handler(void)
{
    if (esp8266_wifi_connect_status() == !W_OK)
        ESP8266_Status.error_code |= ERROR_WiFi_CONNECT, esp8266_log_print("ERROR_WiFi_CONNECT:	WIFI DISCONNECT\r\n");
    else if (esp8266_wifi_connect_status() == W_OK)
        ESP8266_Status.error_code &= ~ERROR_WiFi_CONNECT, esp8266_log_print("WIFI GOT IP\r\n");

    if (esp8266_mqtt_connect_status() == !W_OK)
        ESP8266_Status.error_code |= ERROR_MQTT_CONNECT, esp8266_log_print("ERROR_MQTT_CONNECT:	MQTT DISCONNECT\r\n");
    else if (esp8266_mqtt_connect_status() == W_OK)
        ESP8266_Status.error_code &= ~ERROR_MQTT_CONNECT, esp8266_log_print("+MQTTCONNECTED:0,1,\"mqtts.heclouds.com\",\"1883\",\"\",1\r\n");
#if ONENET_MQTT_SET_ENABLE
    if (esp8266_get_onenet_command_flag() == W_OK) // 判断是命令数据
    {
        esp8266_processing_command(); // 处理数据
        esp8266_command_control();    // 执行命令
        esp8266_clear_buffer();       // 清除无线模块接收数据缓存
        esp8266_publish_data();       // 执行完成后发送数据
    }
#endif
    esp8266_clear_buffer(); // 清除无线模块接收数据缓存
}
void uart_to_onenet(void)
{
    esp8266_system_handler();      // 执行无线模块相关事件
    esp8266_onenet_data_handler(); // 处理有关onenet的数据
    if ((ESP8266_Status.error_code & ERROR_MQTT_CONNECT) == 0)
        esp8266_publish_data(); // 发布数据测试
}
