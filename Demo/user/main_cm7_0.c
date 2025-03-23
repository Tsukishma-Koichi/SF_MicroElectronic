#include "zf_common_headfile.h"
#include "headfile.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完


// **************************** 代码区域 ****************************
#define PIT0                             (PIT_CH0 )                             // 使用的周期中断编号
#define PIT1                             (PIT_CH1 )
uint8 pit_00_state = 0;
uint8 pit_01_state = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_init();                  // 调试串口信息初始化
    OSC_Init();
    pit_ms_init(PIT0, 5);
    pit_ms_init(PIT1, 100);
    WLUART_Init();
    Encoder_Init();
    Motor_Init();
    UART_Init();
    
    // PID初始化
    Motor_PID_Init();

    Motor1_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);
    Motor2_PID_Set(MOTOR_PID_P, MOTOR_PID_I, MOTOR_PID_D, MOTOR_PID_SL, MOTOR_PID_UL, 1);

    


    while(true)
    {
      if(pit_00_state)
        {

          
            MotorR_PIDwork(200);
//            MotorL_PIDwork(200); 
//            Motor_L(1, 2000);
//            Motor_R(1, 2000);
            pit_00_state = 0;
        }
        
        if(pit_01_state)
        {
            OSC_Send(200, Encoder_GetCnt(L), Encoder_GetCnt(R));
            
            pit_01_state = 0;
        }

        
//        system_delay_ms(50);
      

    }
}

// **************************** 代码区域 ****************************
// *************************** 例程硬件连接说明 ***************************
// 使用 cyt4bb7 核心板直接接线进行测试
//      模块管脚            单片机管脚
//      1DIR                P09_1 或者 P05_1
//      1PWM                P09_0 或者 P05_0
//      GND                 GND
//      2DIR                P10_3 或者 P05_3
//      2PWM                P10_2 或者 P05_2
//      GND                 GND
//      接线端子 +          电池正极
//      接线端子 -          电池负极
// 使用 cyt4bb7 学习主板进行测试
//      将模块的电源接线端子与主板的驱动供电端子连接
//      将模块的信号接口使用配套灰排线与主板电机信号接口连接 请注意接线方向 不确定方向就是用万用表确认一下 引脚参考上方核心板连接
//      将主板与供电电池正确连接
// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
// 问题1：电机不转或者模块输出电压无变化
//      如果使用主板测试，主板必须要用电池供电
//      检查模块是否正确连接供电 必须使用电源线供电 不能使用杜邦线
//      查看程序是否正常烧录，是否下载报错，确认正常按下复位按键
//      万用表测量对应 PWM 引脚电压是否变化，如果不变化证明程序未运行，或者引脚损坏，或者接触不良 联系技术客服