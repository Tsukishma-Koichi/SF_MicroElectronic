/*********************************************************************************************************************
* CYT4BB Opensourec Library 即（ CYT4BB 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是 CYT4BB 开源库的一部分
*
* CYT4BB 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main_cm7_1
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          IAR 9.40.1
* 适用平台          CYT4BB
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期              作者                备注
* 2024-1-4       pudding            first version
********************************************************************************************************************/

#include "zf_common_headfile.h"
#include "headfile1.h"
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完

// 本例程是开源库空工程 可用作移植或者测试各类内外设

// **************************** 代码区域 ****************************



//int m7_1_data;          //发给0核数据

/* ================================
   定义 M7_0 数据数组   
   序号 0：        位置偏移    
   序号 1：        元素状态位   
   序号 2：        左右区分    左 1  右 2  不区分 0  
   序号 3：        固定打角
   序号 4：        待定
==================================*/

int search_line_mode = NORMAL_MODE;
//int search_line_mode = LEFT_MODE;

uint8 L_boundary[MT9V03X_H], R_boundary[MT9V03X_H], M_boundary[MT9V03X_H];             // 边界及中线数组

static uint8 filted_buffer[188*120];      // 滤波后缓冲区   
static uint8 binary_buffer[188*120];      // 二值化缓冲区
uint8 img[MT9V03X_H][MT9V03X_W];          // 预处理后图像

uint8 threshold = 200;                    // 二值化阈值初始化

ELEMENT_STATE E;       // 元素状态结构体声明
ROUND R;            // 环岛状态结构体声明
 

int main(void)
{
    Main_Init();                  // 总初始化
    
    State_Clear(&E);              // 元素状态初始化
    Round_Clear(&R);              // 环岛状态初始化
    
    while(true)         //一键启动
    {
      SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
        if(m7_0_data[4] == 1)         // 获取 KEYx 电平为低
        {
//          printf("%d", m7_0_data[4]);
            break;
        }
    }
    system_delay_ms(100);
    
    while(true)
    {
        // 接受到新的帧
      
        
        if (mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            
            // 图像处理
            mean_filter((uint8 *)mt9v03x_image, (uint8 *)filted_buffer);               // 图像滤波
            image_binarize((uint8 *)mt9v03x_image, binary_buffer, MT9V03X_W, MT9V03X_H, threshold);             // 二值化
            memcpy(img[0], &binary_buffer[0], MT9V03X_IMAGE_SIZE);               // 图像备份再发送，以避免图像撕裂
            
            // 扫线
            if(search_line_mode == NORMAL_MODE) mid_seek_boundary(img);
            else LR_seek_boundary(img);
            
            // 元素识别
            Roundabout_detect(img, &R, &E);
            StartStopZone_detect(&E);
            Bend_detect(&E);
            OpenRoad_detect(&E, img);
            
//            // 发送位置偏差
            m7_0_data[0] = get_offset(&E);    
            SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
            
            // 显示图像
            ips114_displayimage03x((const uint8 *)img, MT9V03X_W, MT9V03X_H);    //显示图像
           
//            get_angel();
//            printf("G:%f\tF:%f\n", Gyro_z, filtered_z_gyro);
        }
       //get_threshold();          // 计算二值化阈值
    }
}
