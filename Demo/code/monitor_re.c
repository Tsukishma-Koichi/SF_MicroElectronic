#include "monitor_re.h"

//int m7_1_data;

// 定义数据接收回调函数 如果另外一个核心发送信息 此核心会触发中断并且可以在回调函数读取数据
//void my_ipc_callback(int receive_data)
//{
//    m7_1_data = receive_data;
////    printf("receive data:%d\r\n", receive_data);        // 将接收到的数据打印到串口 
////    printf("Diff:%d\r\n", Diff_Data);
//}



//------------------------------------------------------------
//状态机
uint8 Monitor_readFinishStatus = 0;
//------------------------------基础数据------------------------------
//数据
int16 Monitor_DataRead = 0;                        //采集到的数据
int16 Monitor_Filter[MONITOR_FILTER_MAX + 1];   //滤波队列缓冲区
int16 Monitor_dataPointer = 0;                      //做一个指针,做个循环队列
int16 Monitor_Data = 0;                            //滤波后的数据
//------------------------------
//数据滤波时候的权重
float Monitor_filterWeight[MONITOR_FILTER_MAX] = {0.8, 0.1, 0.06, 0.04};

/**
 * @brief   读取一次数据并进行滤波
 * @return  NULL
 */
void Monitor_ReRead(void) {
    Monitor_readFinishStatus = 0;
    //数据采集的工作
    
    Monitor_DataRead = m7_0_data[0];
    Monitor_Data = 0;
    //进行一个滑动窗口的滤波
    Monitor_Filter[Monitor_dataPointer] = Monitor_DataRead;
    for (int i = 0; i < MONITOR_FILTER_MAX; ++i) {
        int index = Monitor_dataPointer - i;
        if (index < 0)
            index += MONITOR_FILTER_MAX;
        Monitor_Data += Monitor_Filter[index] * Monitor_filterWeight[i];
    }
    Monitor_dataPointer = (++Monitor_dataPointer) % MONITOR_FILTER_MAX;
    Monitor_readFinishStatus = 1;
}