#include "osc.h"

// ��ʼ���������ʾ�����Ľṹ��
seekfree_assistant_oscilloscope_struct oscilloscope_data;

void OSC_Init(void)
{
    // �����������ʹ��DEBUG���ڽ����շ�
//    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIRELESS_UART);
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    
    

    oscilloscope_data.data[0] = 200.0;
    oscilloscope_data.data[1] = 0;
    // ����Ϊ4��ͨ����ͨ���������Ϊ8��
    oscilloscope_data.channel_num = 2;

}

void OSC_Send(uint16 target, uint16 Data)
{
    seekfree_assistant_oscilloscope_send(&oscilloscope_data);
    printf("Encodeder:%d,%d\n", target, Data);
//            oscilloscope_data.data[0] = 200.0;
//            oscilloscope_data.data[1] = 1.0*Encoder_GetCnt(R);
}