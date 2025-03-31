#include "atgm336h_app.h"

// 定义一个函数，用于处理GPS数据，参数为接收到的串口数据缓冲区和指向经纬度数据结构体的指针
void atgm336h_process(unsigned char *uartReadBuffer, Sensor_data *data)
{
    // 查找字符串"$GNGGA"和"\r\n$GNGLL"，它们分别标记GPS数据的开始和结束
    char *start = strstr((const char *)uartReadBuffer, "$GNGGA");
    char *end = strstr((const char *)uartReadBuffer, "\r\n$GNGLL");

    // 如果没有找到这两个标记，说明没有有效的GPS数据
    if (start == NULL || end == NULL)
    {
        // 打印提示信息，表示未找到GNGGA数据行
        printf("[GPS]$GNGGA line NOT found\r\n");
    }
    else
    {
        // 成功找到开始和结束标记，提取数据
        // 创建一个字符数组，用于存储提取的GNGGA数据
        char gngga[100];

        // 从缓冲区中复制从开始到结束之间的数据到gngga数组
        strncpy(gngga, start, end - start);
        gngga[end - start] = '\0'; // 确保字符串以空字符结尾

        // 打印提取的GNGGA数据，用于调试
        // printf("---%s\r\n", gngga);

        // 定义一个指针，用于分割字符串，以及一个数组用于存储分割后的字段
        char *token;
        token = strtok(gngga, ","); // 使用逗号作为分隔符
        char *nmea_fields[15];      // 最多支持15个字段
        int i = 0;

        // 遍历分割后的字段，存储到nmea_fields数组中
        while (token != NULL)
        {
            nmea_fields[i] = token;
            token = strtok(NULL, ","); // 获取下一个字段
            i++;
            if (i >= 15)
                break; // 限制字段数量为15
        }

        // 如果提取的字段数量大于6，说明数据有效
        if (i > 6)
        {
            // 成功提取数据，处理纬度和经度

            // 提取纬度的度部分
            float lat_deg = atof(nmea_fields[2]) / 100; // 例如：2056.122314 -> 20
            // 提取纬度的分部分
            float lat_min = atof(nmea_fields[2]) - (lat_deg * 100); // 例如：2056.122314 - 20*100 = 56.122314

            // 计算十进制度数格式的纬度
            data->latitude = lat_deg + (lat_min / 60);
            // 如果是南纬，设置为负值
            if (nmea_fields[3][0] == 'S')
                data->latitude = -data->latitude;
            // 提取经度的度部分
            float lon_deg = atof(nmea_fields[4]) / 100; // 例如：11002.398438 -> 110
            // 提取经度的分部分
            float lon_min = atof(nmea_fields[4]) - (lon_deg * 100); // 例如：11002.398438 - 110*100 = 2.398438

            // 计算十进制度数格式的经度
            data->longitude = lon_deg + (lon_min / 60);
            // 如果是西经，设置为负值
            if (nmea_fields[5][0] == 'W')
                data->longitude = -data->longitude;
            // 打印`转换后的纬度和经度数据，用于调试
            // printf("[GPS]Latitude: %.6f, Longitude: %.6f\r\n", data->latitude, data->longitude);
        }
        else
        {
            // 如果字段数量不足6，说明数据无效
            // printf("[GPS]data NOT found\r\n");
        }
    }
    // printf("---GPS_END---\r\n"); // 调试用的结束标记
}

// 定义一个任务函数，用于处理GPS数据
void atgm336h_proc(void)
{
    // 检查是否收到了串口数据（rx6_flag为1表示有数据）
    if (rx6_flag == 1)
    {
        	printf("uart6_dma_buffer:%s\r\n",uart6_dma_buffer);
        // 调用GPS数据处理函数，传入接收到的数据缓冲区和经纬度数据结构体指针
        atgm336h_process((unsigned char *)uart6_dma_buffer, &sensor);
        // 清空数据缓冲区
        memset(uart6_dma_buffer, 0, sizeof(uart6_dma_buffer));
        // 重置接收标志
        rx6_flag = 0;
    }
}
