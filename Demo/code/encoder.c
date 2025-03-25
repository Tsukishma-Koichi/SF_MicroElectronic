#include "encoder.h"

int16 encoder_data_quad[2] = {0};



void Encoder_Init(void)
{
	encoder_quad_init(ENCODER_QUAD1, ENCODER_QUAD1_PHASE_A, ENCODER_QUAD1_PHASE_B);  // 初始化编码器模块与引脚 正交编码器模式
	encoder_quad_init(ENCODER_QUAD2, ENCODER_QUAD2_PHASE_A, ENCODER_QUAD2_PHASE_B);  // 初始化编码器模块与引脚 正交编码器模式
	
	                                                     // 初始化 PIT0 为周期中断 100ms 周
}


void Encoder_print(void)
{
	    encoder_data_quad[0] = -encoder_get_count(ENCODER_QUAD1);
	    encoder_clear_count(ENCODER_QUAD1);
	    encoder_data_quad[1] = encoder_get_count(ENCODER_QUAD2);
	    encoder_clear_count(ENCODER_QUAD2);
	    
	    printf("LCounter \t\t%04d\r\n", encoder_data_quad[0]);      // 输出编码器计数信息
        
        printf("RCounter \t\t%04d\r\n", encoder_data_quad[1]);      // 输出编码器计数信息

}


uint16 Encoder_GetCnt(uint8 RorL)
{
    switch (RorL)
    {
    case 'L':
      {
        encoder_data_quad[0] = -encoder_get_count(ENCODER_QUAD1);
        encoder_clear_count(ENCODER_QUAD1);
        return encoder_data_quad[0];
      }
    case 'R':
      {
        encoder_data_quad[1] = encoder_get_count(ENCODER_QUAD2);
        encoder_clear_count(ENCODER_QUAD2);
        return encoder_data_quad[1];
      }
    }
    return 0;
}

//------------------------------------------------------------
//电机状态机
uint8 Encoder_readFinishStatus = 0;

//------------------------------基础数据------------------------------
//电机数据
int16 Encoder_1DataRead = 0;                        //电机1采集到的数据
int16 Encoder_2DataRead = 0;                        //电机2采集到的数据
int16 Encoder_leftFilter[ENCODER_FILTER_MAX + 1];   //左轮滤波队列缓冲区
int16 Encoder_rightFilter[ENCODER_FILTER_MAX + 1];  //右轮滤波队列缓冲区
int16 Encoder_dataPointer = 0;                      //做一个指针,做个循环队列
int16 Encoder_1Data = 0;                            //电机1滤波后的数据
int16 Encoder_2Data = 0;                            //电机2滤波后的数据
//------------------------------
//数据滤波时候的权重
float Encoder_filterWeight[ENCODER_FILTER_MAX] = {0.8, 0.1, 0.06, 0.04};
       

/**
 * @brief   编码器读取一次数据并进行滤波
 * @return  NULL
 */
void Encoder_SpeedRead(void) {
    Encoder_readFinishStatus = 0;
    //数据采集的工作
    Encoder_1DataRead = encoder_get_count(ENCODER_QUAD1);
    Encoder_2DataRead = encoder_get_count(ENCODER_QUAD2);
    encoder_clear_count(ENCODER_QUAD1);
    encoder_clear_count(ENCODER_QUAD2);
    //这里是做编码器的矫正的问题 ->  否则会显示负数据(很怪)
    Encoder_1DataRead = -Encoder_1DataRead;
    Encoder_1Data = 0;
    Encoder_2Data = 0;
    //进行一个滑动窗口的滤波
    Encoder_leftFilter[Encoder_dataPointer] = Encoder_1DataRead;
    Encoder_rightFilter[Encoder_dataPointer] = Encoder_2DataRead;
    for (int i = 0; i < ENCODER_FILTER_MAX; ++i) {
        int index = Encoder_dataPointer - i;
        if (index < 0)
            index += ENCODER_FILTER_MAX;
        Encoder_1Data += Encoder_leftFilter[index] * Encoder_filterWeight[i];
        Encoder_2Data += Encoder_rightFilter[index] * Encoder_filterWeight[i];
    }
    Encoder_dataPointer = (++Encoder_dataPointer) % ENCODER_FILTER_MAX;
    Encoder_readFinishStatus = 1;
}

