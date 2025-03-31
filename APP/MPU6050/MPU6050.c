#include "MPU6050.h"
MPU_TypeDef mpu_data;
unsigned long walk;
float steplength = 0.3, Distance; //
uint8_t svm_set = 1;              //
HAL_StatusTypeDef mpu_write_len(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data)
{
    if (len == 0 || data == NULL)
    {
        return HAL_ERROR;
    }
    return HAL_I2C_Mem_Write(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 0x100);
}
HAL_StatusTypeDef mpu_read_len(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data)
{
    if (len == 0 || data == NULL)
    {
        return HAL_ERROR;
    }
    return HAL_I2C_Mem_Read(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 0x100);
}
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    if (HAL_I2C_Master_Transmit(&hi2c1, (MPU_ADDR), buf, 2, HAL_MAX_DELAY) != HAL_OK)
    {
        return 1; // fail
    }
    return 0; // success
}

uint8_t MPU_Read_Byte(uint8_t reg)
{
    uint8_t data;
    // ??€?????
    if (HAL_I2C_Master_Transmit(&hi2c1, (MPU_ADDR), &reg, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return 0; // fail
    }

    // read data
    if (HAL_I2C_Master_Receive(&hi2c1, (MPU_ADDR), &data, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        return 0; // fail
    }
    return data; // read success
}
/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = {0, 1, 0,
                                          -1, 0, 0,
                                          0, 0, 1};

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7; // error
    return b;
}

static unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;

    return scalar;
}

static int run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x3)
    {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        // dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel_sens = 0; //???????0,??????
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        // dmp_set_accel_bias(accel);
    }
    else
    {
        return -1;
    }

    return 0;
}

int mpu_dmp_init(void)
{
    int ret;
    struct int_param_s int_param;
    // mpu_init
    ret = mpu_init(&int_param);
    if (ret != 0)
    {
        printf("1");
        return ERROR_MPU_INIT;
    }
    //?????
    ret = mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    if (ret != 0)
    {
        printf("2");
        return ERROR_SET_SENSOR;
    }
    // fifo
    ret = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
    if (ret != 0)
    {
        printf("3");
        return ERROR_CONFIG_FIFO;
    }
    //?????
    ret = mpu_set_sample_rate(DEFAULT_MPU_HZ);
    if (ret != 0)
    {
        printf("4");
        return ERROR_SET_RATE;
    }
    // ??????dmp????
    ret = dmp_load_motion_driver_firmware();
    if (ret != 0)
    {
        printf("5");
        return ERROR_LOAD_MOTION_DRIVER;
    }
    // ??????dmp??????
    ret = dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
    if (ret != 0)
    {
        printf("6");
        return ERROR_SET_ORIENTATION;
    }
    // ??????dmp??????
    ret = dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                             DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL |
                             DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL);
    if (ret != 0)
    {
        printf("7");
        return ERROR_ENABLE_FEATURE;
    }
    // ??????????????
    ret = dmp_set_fifo_rate(DEFAULT_MPU_HZ);
    if (ret != 0)
    {
        printf("8");
        return ERROR_SET_FIFO_RATE;
    }
    // ????
    ret = run_self_test();
    //		if (ret != 0)
    //        {
    //           printf("9");
    //            return ERROR_SELF_TEST;
    //        }
    // ????
    ret = mpu_set_dmp_state(1);
    if (ret != 0)
    {
        printf("10");
        return ERROR_DMP_STATE;
    }
    return 0;
}
int MPU6050_DMP_Get_Data(MPU_TypeDef *data)
{
    float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    short gyro[3];
    short accel[3];
    long quat[4];
    unsigned long timestamp;
    short sensors;
    unsigned char more;

    if (dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, &more))
    {
        printf("nbde\r\n");
        return -1;
    }

    if (sensors & INV_WXYZ_QUAT)
    {
        q0 = quat[0] / Q30;
        q1 = quat[1] / Q30;
        q2 = quat[2] / Q30;
        q3 = quat[3] / Q30;
        data->pitch = asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;                                    // pitch
        data->roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3;     // roll
        data->yaw = atan2(2 * (q0 * q3 + q1 * q2), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3; // yaw
    }
    return 0;
}
int MPU6050_Init(void)
{
    MPU_Write_Byte(MPU6050_PWR_MGMT_1, 0x00);   // ????
    MPU_Write_Byte(MPU6050_SMPLRT_DIV, 0x07);   //
    MPU_Write_Byte(MPU6050_CONFIG, 0x06);       // ???????????
    MPU_Write_Byte(MPU6050_GYRO_CONFIG, 0x18);  // ?????????????????????????
    MPU_Write_Byte(MPU6050_ACCEL_CONFIG, 0x01); // ?????????????????
    return 0;
}

/*?????????????????????*/
uint8_t MPU_Get_Gyroscope(MPU_TypeDef *data)
{
    uint8_t buf[6], res;
    res = mpu_read_len(MPU_ADDR, MPU6050_GYRO_XOUT_H, 6, buf);
    if (res == 0)
    {
        data->gx = ((uint16_t)buf[0] << 8) | buf[1];
        data->gy = ((uint16_t)buf[2] << 8) | buf[3];
        data->gz = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
    ;
}
/*???????????????*/
uint8_t MPU_Get_Accelerometer(MPU_TypeDef *data)
{
    uint8_t buf[6], res;
    res = mpu_read_len(MPU_ADDR, MPU6050_ACCEL_XOUT_H, 6, buf);
    if (res == 0)
    {
        data->ax = ((uint16_t)buf[0] << 8) | buf[1];
        data->ay = ((uint16_t)buf[2] << 8) | buf[3];
        data->az = ((uint16_t)buf[4] << 8) | buf[5];
    }
    return res;
}
void dmp_proc(void)
{
    MPU6050_DMP_Get_Data(&mpu_data);
}
