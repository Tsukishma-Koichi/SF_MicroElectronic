#include "headfile.h"
#define LED1  (P19_0)


// 双核通信
void my_ipc_callback(int receive_data) 
{
    //接受到数据后的操作
    
}


// 总初始化
void Main_Init(void)
{                  
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化
    debug_init();                       // 调试串口信息初始化
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);      // 初始化 LED1 输出 默认高电平 推挽输出模式
    
    // 中断初始化
    pit_ms_init(PIT_CH0, 5000);
    // 设置逐飞助手使用DEBUG串口进行收发
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    // 双核通信初始化
    SCB_DisableDCache(); // 关闭DCashe
    ipc_communicate_init(IPC_PORT_2, my_ipc_callback);  // 初始化IPC模块 选择端口2 填写中断回调函数
    
    //  ips114初始化
    ips114_init();
    ips114_show_string(0, 0, "mt9v03x init.");
    while(1)
    {
        if(mt9v03x_init())
            ips114_show_string(0, 16, "mt9v03x reinit.");
        else
            break;
        system_delay_ms(100);                             // 短延时快速闪灯表示异常
    }
    ips114_show_string(0, 16, "init success.");
  
    //  摄像头初始化
    while(1)
    {
        if(mt9v03x_init())
            gpio_toggle_level(LED1);                                         // 翻转 LED 引脚输出电平 控制 LED 亮灭 初始化出错这个灯会闪的很慢
        else
            break;
        system_delay_ms(500);                                                   // 闪灯表示异常
    }
}



















