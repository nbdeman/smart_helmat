#ifndef __ESP_8266_H__
#define __ESP_8266_H__

#include "bsp_system.h"
#include <stdint.h>
/*串口返回句柄*/
typedef struct
{
    uint8_t receiveDataFlag; // 接收数据标志位
    uint16_t error_code;     // 无线模块初始化错误代码
} ESP8266_TypeDef;
extern ESP8266_TypeDef ESP8266_Status;
void esp8266_init(void);
uint8_t esp8266_mqtt_connect_status(void);
uint8_t esp8266_wifi_connect_status(void);
uint8_t esp8266_get_receive_flag(void);
uint8_t esp8266_send_command(char *cmd, char *res, uint8_t sendCount, uint8_t clear_buffer, uint16_t delay_xms, uint8_t printf_enable);
void esp8266_init(void);
void esp8266_error_handler(uint16_t error_code);
void esp8266_system_handler(void);
void esp8266_onenet_data_handler(void);
void esp8266_publish_data(void);
uint8_t esp8266_get_onenet_command_flag(void);
uint32_t esp8266_get_command_id(char *jsonData);
void esp8266_receiver_command_extract(char *jsonData);
void esp8266_receive_command_respond(uint32_t respond_id, uint16_t code, char *msg);
void esp8266_command_control(void);
void esp8266_processing_command(void);
void esp8266_receive_data_handler(void);
void esp8266_receive_callback(uint8_t byte);
void uart_to_onenet(void);
// 定义联合体，用于存储不同类型的数据
typedef union
{
    uint8_t bool_value;       // 布尔型
    int16_t int_value;        // 整型
    float float_value;        // 浮点型
    const char *string_value; // 字符串
} Type_Value;
/*上报数据名称及数据类型*/
typedef struct
{
    char *name;       // 数据名称
    uint8_t type;     // 数据类型
    Type_Value value; // 数据值
    float min_value;  // 数据最小值
    float max_value;  // 数据最大值
} OneNET_MQTT_Data;

#define W_ENABLE 1
#define W_DISABLE 0

#define W_FALSE 0
#define W_TRUE 1

#define W_OK 1
#define ERROR_WiFi_CONNECT (0x01 << 15)
#define ERROR_MQTT_CONNECT (0x01 << 14)
#define ERROR_PUBLISH (0x01 << 13)
// 定义类型标识常量
#define TYPE_BOOL (0x01 << 0)
#define TYPE_INT (0x01 << 1)
#define TYPE_FLOAT (0x01 << 2)
#define TYPE_STRING (0x01 << 3)
/**
 * @简要  	wifi信息和onenet云平台产品信息
 * @注意	需要根据自己的信息填写，连接2.4GHz频段的无线网络，token根据token生成工具填写规则生成
 */
#define ONENET_MQTT_SET_ENABLE 0            // 使能属性设置功能  	1-开启	0-关闭 	如不需使用属性设置功能，可以不使用cJson减少内存开销，以及加快运行速度
#define WIFI_SSID "nbdeman"                 // WIFI用户名
#define WIFI_PASSWORD "123456789"           // WIFI密码
#define ONENET_MQTT_PRODUCT_ID "c7EW4zM74R" // OneNET MQTT产品ID
#define ONENET_MQTT_DEVICE_NAME "adc"       // OneNET MQTT设备名称
#define ONENET_MQTT_TOKEN "version=2018-10-31&res=products%2Fc7EW4zM74R%2Fdevices%2Fadc&et=2088447118&method=md5&sign=1npL1hWGcoF4UOeAnSvzEQ%3D%3D"
/**
 * @简要  	连接onenet以及下发数据和上报数据的AT指令集
 * @注意	这里一般无需更改，如果需要其他AT指令参数，可以在下方对应的位置更改参数
 */
#define WIRELESS_WIFI_INFO "AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PASSWORD "\"\r\n"                                                                           // AT指令：连接2.4GHz wifi
#define ONENET_MQTT_USERCFG_INFO "AT+MQTTUSERCFG=0,1,\"" ONENET_MQTT_DEVICE_NAME "\",\"" ONENET_MQTT_PRODUCT_ID "\",\"" ONENET_MQTT_TOKEN "\",0,0,\"\"\r\n" // AT指令：配置MQTT客户端的用户参数
#define ONENET_MQTT_SERVER_INFO "AT+MQTTCONN=0,\"mqtts.heclouds.com\",1883,1\r\n"                                                                           // AT指令：连接onenet的mqtt服务器
#define ONENET_MQTT_REPLY_TOPIC "AT+MQTTSUB=0,\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/post/reply\",0\r\n"              // AT指令：设备上报响应请求（订阅）
#define ONENET_MQTT_PUBTOPIC "AT+MQTTPUBRAW=0,\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/post\""                          // AT指令：设备属性上报请求（发布）
#define ONENET_MQTT_PUB_SET "AT+MQTTPUB=0,\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/set_reply\""                         // AT指令：设备属性设置响应（发布）

#if ONENET_MQTT_SET_ENABLE
#include "cJson.h"                                                                                                                    //如果不使用属性设置，可以不用cJson文件代码
#define ONENET_MQTT_SET_TOPIC "AT+MQTTSUB=0,\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/set\",0\r\n" // AT指令：设备属性设置请求（订阅）
#define ONENET_MQTT_REVEIVE_SET_TOPIC "\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/set\""            // 属性设置请求接收判断
#define ONENET_MQTT_REVEIVE_REPLY_TOPIC "\"$sys/" ONENET_MQTT_PRODUCT_ID "/" ONENET_MQTT_DEVICE_NAME "/thing/property/post/reply\""   // 属性上报请求接收判断
#endif
#endif
