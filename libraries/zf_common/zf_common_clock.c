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
* 文件名称          zf_common_clock
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

#include "system_cyt4bb.h"
#include "zf_common_interrupt.h"
#include "zf_common_clock.h"



uint32 system_clock = 0;                                          	        // 系统时钟信息

extern void system_delay_init(void);
    
//-------------------------------------------------------------------------------------------------------------------
// 函数简介     核心时钟恢复初始设置
// 参数说明     void
// 返回参数     void
// 使用示例     clock_reset();
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
void clock_reset (void)
{
    // 暂未实现
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     核心时钟设置
// 参数说明     clock       时钟频率 推荐使用 zf_common_clock.h 中 system_clock_enum 定义的选项
// 返回参数     void
// 使用示例     clock_set_freq(clock);
// 备注信息     本函数在文件内部调用 用户不用关注 也不可修改
//-------------------------------------------------------------------------------------------------------------------
void clock_set_freq (uint32 clock)
{
    // 暂未实现
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     核心时钟初始化
// 参数说明     clock       时钟频率 推荐使用 zf_common_clock.h 中 system_clock_enum 定义的选项
// 返回参数     void
// 使用示例     clock_init(SYSTEM_CLOCK_250M);                  // 初始化核心时钟为 250MHz
// 备注信息     本函数用于将核心时钟设置到指定频率
//-------------------------------------------------------------------------------------------------------------------
void clock_init (uint32 clock)
{
   system_clock = clock;                                                        // 记录核心时钟频率
   SystemInit();                                                                // 系统初始化
   system_delay_init();							        // 定时器延时初始化
   interrupt_global_enable(0);							// 开启全局中断
}
