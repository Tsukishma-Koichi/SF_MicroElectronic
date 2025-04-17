

#include "gyro.h"

//------------------------------基本变量------------------------------
// 计时时间 - 单位 ms
uint16 Gyroscope_time = 0;
// 使用的陀螺仪设备(默认设置为IMU660RA)
GYROSCOPE_TYPE Gyroscope_device = GYROSCOPE_IMU660RA;
// 陀螺仪偏移量
struct GyroscopeOffset Gyro_Offset;

// 定义Z轴专用缓冲区和权重
float z_gyro_buffer[GYRO_FILTER_SIZE] = {0};  // 初始化为0
float z_weights[GYRO_FILTER_SIZE] = {0.05,0.05,0.1,0.1,0.1,0.15,0.15,0.15,0.1,0.05};  // 权重总和≈1，越新的数据权重越高
uint8_t z_data_ptr = 0;  // 环形缓冲区指针（必须为全局变量或静态变量，确保持久化）

//------------------------------计数变量------------------------------
//------------------------------
// 存储数据变量
float Gyro_x; // 陀螺仪x值 - 角度 单位°
float Gyro_y; // 陀螺仪y值 - 角度 单位°
float Gyro_z; // 陀螺仪z值 - 角度 单位°
float Acc_x;  // 加速度x值 - 速度 单位m/s
float Acc_y;  // 加速度y值 - 速度 单位m/s
float Acc_z;  // 加速度z值 - 速度 单位m/s

float Gyro_corrX; // 陀螺仪x值 - 角速度 单位°/s
float Gyro_corrY; // 陀螺仪y值 - 角速度 单位°/s

float Gyro_corrZ; // 陀螺仪z值 - 角速度 单位°/s
float filtered_z_gyro;

float Acc_corrX;  // 加速度x值 - 加速度 单位m/s^2
float Acc_corrY;  // 加速度y值 - 加速度 单位m/s^2
float Acc_corrZ;  // 加速度z值 - 加速度 单位m/s^2
float test =0;

//------------------------------
// 计数状态机
uint8 Gyro_x_status = 0; // 陀螺仪x值计数状态机
uint8 Gyro_y_status = 0; // 陀螺仪y值计数状态机
uint8 Gyro_z_status = 0; // 陀螺仪z值计数状态机
uint8 Acc_x_status = 0;  // 加速度x值计数状态机
uint8 Acc_y_status = 0;  // 加速度y值计数状态机
uint8 Acc_z_status = 0;  // 加速度z值计数状态机

/**
 * @brief               陀螺仪处理初始化
 * @param device        使用的陀螺仪设备
 * @param time          使用的中断时间，单位为毫秒
 * @note                z轴角速度左转为正，右转为负
 * @example             Gyroscope_Init(GYROSCOPE_IMU660RA, 10);
 */
void Gyroscope_Init(GYROSCOPE_TYPE device, uint16 time)
{
    // 陀螺仪偏移量初始量设置为0
    Gyro_Offset.ACC_Xdata = 0.0;
    Gyro_Offset.ACC_Ydata = 0.0;
    Gyro_Offset.ACC_Zdata = 0.0;
    Gyro_Offset.Gyro_Xdata = 0.0;
    Gyro_Offset.Gyro_Ydata = 0.0;
    Gyro_Offset.Gyro_Zdata = 0.0;

    // 处理IMU660RA的代码
    if (device == GYROSCOPE_IMU660RA)
    {
        Gyroscope_device = GYROSCOPE_IMU660RA;
        // 数据点循环采样150次,每采样一次,系统延时10ms,所以共需要延时1.5s
        for (uint8 i = 0; i < 150; ++i)
        {
            imu660ra_get_gyro();
            imu660ra_get_acc();
            Gyro_Offset.ACC_Xdata += (float)imu660ra_acc_x;
            Gyro_Offset.ACC_Ydata += (float)imu660ra_acc_y;
            Gyro_Offset.ACC_Zdata += (float)imu660ra_acc_z;
            Gyro_Offset.Gyro_Xdata += (float)imu660ra_gyro_x;
            Gyro_Offset.Gyro_Ydata += (float)imu660ra_gyro_y;
            Gyro_Offset.Gyro_Zdata += (float)imu660ra_gyro_z;
            system_delay_ms(10);
        }
        Gyro_Offset.ACC_Xdata /= 150;
        Gyro_Offset.ACC_Ydata /= 150;
        Gyro_Offset.ACC_Zdata /= 150;
        Gyro_Offset.Gyro_Xdata /= 150;
        Gyro_Offset.Gyro_Ydata /= 150;
        Gyro_Offset.Gyro_Zdata /= 150;
    }
    // 处理IMU963RA的代码
    else if (device == GYROSCOPE_IMU963RA)
    {
        Gyroscope_device = GYROSCOPE_IMU963RA;
    }
    // 处理ICM20602的代码
    else if (device == GYROSCOPE_ICM20602)
    {
        Gyroscope_device = GYROSCOPE_ICM20602;
    }
    // 传参数据先不做限制,自己传能用的数据过来~~~
    Gyroscope_time = time;
}

/**
 * @brief                   计时器开始计数
 * @param   measureType     计数类型
 * @example                 Gyroscope_Begin(GYROSCOPE_GYRO_X);
 *
 */
void Gyroscope_Begin(GYROSCOPE_MEASURE_TYPE measureType)
{
    // 要求,只有计数器转态为0的时候,才能开始进行计数
    if (measureType == GYROSCOPE_GYRO_X)
    {
        if (Gyro_x_status == 0)
        {
            Gyro_x_status = 1;
            Gyro_x = 0.0;
        }
    }
    else if (measureType == GYROSCOPE_GYRO_Y)
    {
        if (Gyro_y_status == 0)
        {
            Gyro_y_status = 1;
            Gyro_y = 0.0;
        }
    }
    else if (measureType == GYROSCOPE_GYRO_Z)
    {
        if (Gyro_z_status == 0)
        {
            Gyro_z_status = 1;
//            Gyro_z = 0.0;
        }
    }
    else if (measureType == GYROSCOPE_ACC_X)
    {
        if (Acc_x_status == 0)
        {
            Acc_x_status = 1;
            Acc_x = 0.0;
        }
    }
    else if (measureType == GYROSCOPE_ACC_Y)
    {
        if (Acc_y_status == 0)
        {
            Acc_y_status = 1;
            Acc_y = 0.0;
        }
    }
    else if (measureType == GYROSCOPE_ACC_Z)
    {
        if (Acc_z_status == 0)
        {
            Acc_z_status = 1;
            Acc_z = 0.0;
        }
    }
}

/**
 * @brief                   计数器停止计数
 * @param   measureType     计数类型
 * @example                 Gyroscope_End(GYROSCOPE_GYRO_X);
 *
 */
void Gyroscope_End(GYROSCOPE_MEASURE_TYPE measureType)
{
    // 关掉的话就直接把状态置0了,不用考虑是否处于在工作状态
    if (measureType == GYROSCOPE_GYRO_X)
    {
        Gyro_x_status = 0;
    }
    else if (measureType == GYROSCOPE_GYRO_Y)
    {
        Gyro_y_status = 0;
    }
    else if (measureType == GYROSCOPE_GYRO_Z)
    {
        Gyro_z_status = 0;
    }
    else if (measureType == GYROSCOPE_ACC_X)
    {
        Acc_x_status = 0;
    }
    else if (measureType == GYROSCOPE_ACC_Y)
    {
        Acc_y_status = 0;
    }
    else if (measureType == GYROSCOPE_ACC_Z)
    {
        Acc_z_status = 0;
    }
}

void Gyroscope_GetData(void)
{
    //------------------------------处理数据------------------------------
    imu660ra_get_gyro();
    imu660ra_get_acc();
    Gyro_corrX = imu660ra_gyro_transition((float)imu660ra_gyro_x - Gyro_Offset.Gyro_Xdata);
    Gyro_corrY = imu660ra_gyro_transition((float)imu660ra_gyro_y - Gyro_Offset.Gyro_Ydata);
    
    Gyro_corrZ = imu660ra_gyro_transition((float)imu660ra_gyro_z - Gyro_Offset.Gyro_Zdata);
    filtered_z_gyro = Gyroscope_Filter(
        z_gyro_buffer,  // Z轴专用缓冲区
        z_weights,      // Z轴权重
        GYRO_FILTER_SIZE,
        &z_data_ptr,    // 指针地址
        Gyro_corrZ
    );
    
    Acc_corrX = imu660ra_acc_transition((float)imu660ra_acc_x - Gyro_Offset.ACC_Xdata);
    Acc_corrY = imu660ra_acc_transition((float)imu660ra_acc_y - Gyro_Offset.ACC_Ydata);
    Acc_corrZ = imu660ra_acc_transition((float)imu660ra_acc_z - Gyro_Offset.ACC_Zdata);
}

void Gyroscope_Conut(void)
{
    //------------------------------基本说明------------------------------
    // 进行计数,放中断里面进行数据的累计
    // 速度单位为m/s
    // 角度单位为度
    //--------------------获取数据--------------------
    Gyroscope_GetData();

    //--------------------数据处理--------------------
    if (Gyro_x_status == 1)
    {
        Gyro_x += imu660ra_gyro_transition((float)imu660ra_gyro_x - Gyro_Offset.Gyro_Xdata) * Gyroscope_time * 0.001;
    }

    if (Gyro_y_status == 1)
    {
        Gyro_y += imu660ra_gyro_transition((float)imu660ra_gyro_y - Gyro_Offset.Gyro_Ydata) * Gyroscope_time * 0.001;
    }

    if (Gyro_z_status == 1)
    {
      if (fabs(filtered_z_gyro) >= 5)
        Gyro_z += Gyroscope_time * filtered_z_gyro * 0.01;
    }

    if (Acc_x_status == 1)
    {
        Acc_x += imu660ra_acc_transition((float)imu660ra_acc_x - Gyro_Offset.ACC_Xdata) * Gyroscope_time * 0.001;
    }

    if (Acc_y_status == 1)
    {
        Acc_y += imu660ra_acc_transition((float)imu660ra_acc_y - Gyro_Offset.ACC_Ydata) * Gyroscope_time * 0.001;
    }

    if (Acc_z_status == 1)
    {
        Acc_z += imu660ra_acc_transition((float)imu660ra_acc_z - Gyro_Offset.ACC_Zdata) * Gyroscope_time * 0.001;
    }
}

/**
 * @brief                   清空积分的数据
 * @param measureType       测量的类型
 */
void Gyroscope_Clear(GYROSCOPE_MEASURE_TYPE measureType)
{
    if (measureType == GYROSCOPE_GYRO_X)
    {
        Gyro_x = 0.0;
    }
    else if (measureType == GYROSCOPE_GYRO_Y)
    {
        Gyro_y = 0.0;
    }
    else if (measureType == GYROSCOPE_GYRO_Z)
    {
        Gyro_z = 0.0;
    }
    else if (measureType == GYROSCOPE_ACC_X)
    {
        Acc_x = 0.0;
    }
    else if (measureType == GYROSCOPE_ACC_Y)
    {
        Acc_y = 0.0;
    }
    else if (measureType == GYROSCOPE_ACC_Z)
    {
        Acc_z = 0.0;
    }
}





/**
 * @brief 对陀螺仪的滑动窗口滤波
 * 
 * @param data          数据序列
 * @param data_weigh    滤波权重
 * @param data_num      序列长度
 * @param data_pointer  数据指针
 * @param new_data      新输入的数据
 * @return data_out     滤波输出
 */
float Gyroscope_Filter(float data[], float data_weigh[], uint8 data_num, uint8 *data_pointer, float new_data)
{
    float data_out = 0;
    data[*data_pointer] = new_data;
    for (uint8 i = 0; i < data_num; i++)
    {
        uint8 index = *data_pointer - i;
        if (index < 0)
            index += data_num;
        data_out += data[index] * data_weigh[index];
    }
    *data_pointer = (*data_pointer + 1) % data_num;
    return data_out;
}
