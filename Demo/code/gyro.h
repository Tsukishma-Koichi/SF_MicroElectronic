#ifndef CODE_OPHOT_GYROSCOPE_H_
#define CODE_OPHOT_GYROSCOPE_H_

#include "zf_common_headfile.h"

#include <math.h>
// 定义滤波器参数
#define GYRO_FILTER_SIZE 10  // 缓冲区长度（数据点数）
extern float z_gyro_buffer[GYRO_FILTER_SIZE];
extern float z_weights[GYRO_FILTER_SIZE];
extern uint8_t z_data_ptr;

extern float filtered_z_gyro;

//------------------------------数据类型定义------------------------------
//使用的陀螺仪类型
typedef enum {
    GYROSCOPE_IMU660RA = 0x00,
    GYROSCOPE_IMU963RA = 0x01,
    GYROSCOPE_ICM20602 = 0x02,
}GYROSCOPE_TYPE;

//陀螺仪测量的类型
typedef enum {
    GYROSCOPE_GYRO_X = 0x00,
    GYROSCOPE_GYRO_Y = 0x01,
    GYROSCOPE_GYRO_Z = 0x02,
    GYROSCOPE_ACC_X = 0x03,
    GYROSCOPE_ACC_Y = 0x04,
    GYROSCOPE_ACC_Z = 0x05,
}GYROSCOPE_MEASURE_TYPE;

//陀螺仪偏移量处理
struct GyroscopeOffset {
        float Gyro_Xdata;
        float Gyro_Ydata;
        float Gyro_Zdata;
        float ACC_Xdata;
        float ACC_Ydata;
        float ACC_Zdata;
};


//------------------------------供其它函数调用的数据------------------------------
extern float Gyro_x;           //陀螺仪x值
extern float Gyro_y;           //陀螺仪y值
extern float Gyro_z;           //陀螺仪z值
extern float Acc_x;            //加速度x值
extern float Acc_y;            //加速度y值
extern float Acc_z;            //加速度z值

extern float Gyro_corrX;       //陀螺仪x值 - 角速度
extern float Gyro_corrY;       //陀螺仪y值 - 角速度
extern float Gyro_corrZ;       //陀螺仪z值 - 角速度
extern float Acc_corrX;        //加速度x值 - 加速度
extern float Acc_corrY;        //加速度y值 - 加速度
extern float Acc_corrZ;        //加速度z值 - 加速度
extern float test;

//------------------------------函数------------------------------
void Gyroscope_Init(GYROSCOPE_TYPE device, uint16 time);
void Gyroscope_Begin(GYROSCOPE_MEASURE_TYPE measureType);
void Gyroscope_End(GYROSCOPE_MEASURE_TYPE measureType);
void Gyroscope_GetData(void);
void Gyroscope_Conut(void);
void Gyroscope_Clear(GYROSCOPE_MEASURE_TYPE measureType);
float Gyroscope_Filter(float data[], float data_weigh[], uint8 data_num, uint8 *data_pointer, float new_data);


#endif /* CODE_XIAO_GYROSCOPE_H_ */
