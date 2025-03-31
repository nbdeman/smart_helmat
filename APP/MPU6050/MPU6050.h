#ifndef __MPU6050_H__
#define __MPU6050_H__
#include "bsp_system.h"
typedef struct
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int AVM;
    int GVM;
    float pitch;
    float roll;
    float yaw;
} MPU_TypeDef;
#define MPU6050_SMPLRT_DIV 0x19   // ����������
#define MPU6050_CONFIG 0x1A       // ���üĴ���
#define MPU6050_GYRO_CONFIG 0x1B  // ����������
#define MPU6050_ACCEL_CONFIG 0x1C // ���ٶȼ�����
#define MPU6050_ACCEL_XOUT_H 0x3B // ���ٶȼ�x���λ
#define MPU6050_ACCEL_XOUT_L 0x3C // ���ٶȼ�x���λ
#define MPU6050_ACCEL_YOUT_H 0x3D // y
#define MPU6050_ACCEL_YOUT_L 0x3E // y
#define MPU6050_ACCEL_ZOUT_H 0x3F // z
#define MPU6050_ACCEL_ZOUT_L 0x40 // z
#define MPU6050_TEMP_OUT_H 0x41   // �¶�
#define MPU6050_TEMP_OUT_L 0x42   // ������
#define MPU6050_GYRO_XOUT_H 0x43  //
#define MPU6050_GYRO_XOUT_L 0x44  //
#define MPU6050_GYRO_YOUT_H 0x45  //
#define MPU6050_GYRO_YOUT_L 0x46  //
#define MPU6050_GYRO_ZOUT_H 0x47  //
#define MPU6050_GYRO_ZOUT_L 0x48  //

#define MPU6050_PWR_MGMT_1 0x6B // ��Դ����1
#define MPU6050_PWR_MGMT_2 0x6C // ��Դ����2
#define MPU6050_WHO_AM_I 0x75   // �豸ID
extern MPU_TypeDef mpu_data;

#define MPU_ADDR 0XD0
#define DEFAULT_MPU_HZ (20)
#define Q30 1073741824.0f
#define ERROR_MPU_INIT -1
#define ERROR_SET_SENSOR -2
#define ERROR_CONFIG_FIFO -3
#define ERROR_SET_RATE -4
#define ERROR_LOAD_MOTION_DRIVER -5
#define ERROR_SET_ORIENTATION -6
#define ERROR_ENABLE_FEATURE -7
#define ERROR_SET_FIFO_RATE -8
#define ERROR_SELF_TEST -9
#define ERROR_DMP_STATE -10

HAL_StatusTypeDef mpu_write_len(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data);
HAL_StatusTypeDef mpu_read_len(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data);
int mpu_dmp_init(void);
int MPU6050_DMP_Get_Data(MPU_TypeDef *data);
void dmp_proc(void);
int  MPU6050_Init(void);
/*获取陀螺仪原始值*/
uint8_t MPU_Get_Gyroscope(MPU_TypeDef *data);
/*获取加速度计原始值 */
uint8_t MPU_Get_Accelerometer(MPU_TypeDef *data);
#endif
