#ifndef _MONITOR_RE_H_
#define _MONITOR_RE_H_

#include "zf_common_headfile.h"
#include "headfile0.h"

#define MONITOR_FILTER_MAX          (4)

// ˫��ͨ��
#define DATA_LENGTH               (5)                                           // �������ݳ���
#pragma location = 0x28001000                                                   // ������������鶨�嵽ָ����RAM��ַ��#pragma��Ҫ�ֶ������ַ�������Ҫ�������ݳ��Ⱥ��ٷ���
__no_init int m7_0_data[5];                                          // M7_0��������

//extern int m7_0_data[5];
//extern int m7_1_data[5];

// ����Ԫ��
#define ROUNDA 1
#define BEND 2
#define SSZ 3
#define OR 4 
// ����
#define LEFT 1 
#define RIGHT 2



extern int16 Monitor_Data;




void my_ipc_callback(int receive_data);
void Monitor_ReRead(void);


#endif