#include "osc.h"

// 初始化逐飞助手示波器的结构体
seekfree_assistant_oscilloscope_struct oscilloscope_data;

void OSC_Init(void)
{
    // 设置逐飞助手使用DEBUG串口进行收发
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    
    

//    oscilloscope_data.data[0] = 200.0;
//    oscilloscope_data.data[1] = 0;
    // 设置为4个通道，通道数量最大为8个
    oscilloscope_data.channel_num = 8;

}

void OSC_Send(int Data0, int Data1, int Data2, int Data3, int Data4, int Data5, int Data6, int Data7)
{
    seekfree_assistant_oscilloscope_send(&oscilloscope_data);
    printf("OSC:%d,%d,%d,%d,%d\n", Data0, Data1, Data2, Data3, Data4, Data5, Data6, Data7);

}