#ifndef _MONITOR_RE_H_
#define _MONITOR_RE_H_

#include "zf_common_headfile.h"
#include "headfile0.h"

#define MONITOR_FILTER_MAX          (4)

// 双核通信
#define DATA_LENGTH               (5)                                           // 数组数据长度
#pragma location = 0x28001000                                                   // 将下面这个数组定义到指定的RAM地址，#pragma需要手动分配地址，因此需要计算数据长度后再分配
__no_init int m7_0_data[5];                                          // M7_0数据数组

//extern int m7_0_data[5];
//extern int m7_1_data[5];

// 赛道元素
#define ROUNDA 1
#define BEND 2
#define SSZ 3
#define OR 4 
// 左右
#define LEFT 1 
#define RIGHT 2



extern int16 Monitor_Data;




void my_ipc_callback(int receive_data);
void Monitor_ReRead(void);


#endif