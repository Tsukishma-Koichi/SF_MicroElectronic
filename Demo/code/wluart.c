#include "wluart.h"

void WLUART_Init(void)
{
    if(wireless_uart_init())                                                    // 判断是否通过初始化
    {
        while(1)                                                                // 初始化失败就在这进入死循环
        {
            gpio_toggle_level(LED1);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭
            system_delay_ms(100);                                               // 短延时快速闪灯表示异常
        }
    }
//    wireless_uart_send_byte('\r');
//    wireless_uart_send_byte('\n');
//    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // 初始化正常 输出测试信息
}


/*
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中
// *************************** 例程硬件连接说明 ***************************
// 接入无线转串口模块
//      模块管脚            单片机管脚
//      RX                  查看 zf_device_wrieless_uart.h 中 WIRELESS_UART_RX_PINx  宏定义
//      TX                  查看 zf_device_wrieless_uart.h 中 WIRELESS_UART_TX_PINx  宏定义
//      RTS                 查看 zf_device_wrieless_uart.h 中 WIRELESS_UART_RTS_PINx 宏定义
//      GND                 核心板电源地 GND
//      3V3                 核心板 3V3 电源

// **************************** 代码区域 ****************************
#define LED1                    (P19_0)
uint8 data_buffer[32];
uint8 data_len;


int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_info_init();                  // 调试串口信息初始化
    
    // 此处编写用户代码 例如外设初始化代码等
    uint8 count = 0;

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // 初始化 LED1 输出 默认高电平 推挽输出模式
    if(wireless_uart_init())                                                    // 判断是否通过初始化
    {
        while(1)                                                                // 初始化失败就在这进入死循环
        {
            gpio_toggle_level(LED1);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭
            system_delay_ms(100);                                               // 短延时快速闪灯表示异常
        }
    }
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // 初始化正常 输出测试信息

    // 此处编写用户代码 例如外设初始化代码等
    while(true)
    {
        // 此处编写需要循环执行的代码

        data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);             // 查看是否有消息 默认缓冲区是 WIRELESS_UART_BUFFER_SIZE 总共 64 字节
        if(data_len != 0)                                                       // 收到了消息 读取函数会返回实际读取到的数据个数
        {
            wireless_uart_send_buffer(data_buffer, data_len);                     // 将收到的消息发送回去
            memset(data_buffer, 0, 32);
            func_uint_to_str((char *)data_buffer, data_len);
            wireless_uart_send_string("\r\ndata len:");                                 // 显示实际收到的数据信息
            wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // 显示收到的数据个数
            wireless_uart_send_string(".\r\n");
        }
        if(count ++ > 10)
        {
            count = 0;
            gpio_toggle_level(LED1);                                            // 翻转 LED 引脚输出电平 控制 LED 亮灭
        }
        system_delay_ms(50);

        // 此处编写需要循环执行的代码
    }
}

// **************************** 代码区域 ****************************
// *************************** 例程常见问题说明 ***************************
// 遇到问题时请按照以下问题检查列表检查
// 问题1：串口没有数据
//      查看 逐飞助手 打开的是否是正确的串口，检查打开的 COM 口是否对应的是 无线转串口模块 的 COM 口
//      查看核心板LED灯状态 如果快速闪烁证明初始化失败 检查接线 或者按照说明书检查模块是否正常
// 问题2：串口数据乱码
//      查看是否模块波特率被修改过 恢复出厂设置
// 问题3：发送数据没有收到回复
//      调试看看是否读取到数据 data_len 是否一直都是 如果是 那么按照说明书检查模块是否正常


*/
