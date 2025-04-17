#include "headfile0.h"
#define LED1  (P19_0)


// 总初始化
void Main_Init(void)
{                  
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化
    debug_init();                       // 调试串口信息初始化
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);      // 初始化 LED1 输出 默认高电平 推挽输出模式
    
    // 中断初始化
    pit_ms_init(PIT_CH0, 5000);
    pit_ms_init(PIT_CH1, 5);
    // 设置逐飞助手使用DEBUG串口进行收发
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    
    timer_init(TC_TIME2_CH0, TIMER_MS);                                         // 定时器使用 TC_TIME2_CH0 使用毫秒级计数
    
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
    
    // 陀螺仪初始化
    imu660ra_init();
    Gyroscope_Init(GYROSCOPE_IMU660RA, 5);
    
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

void time_cnt()
{
    timer_start(TC_TIME2_CH0);                                  // 启动定时                                             
    //...
    timer_stop(TC_TIME2_CH0);                                                   // 停止定时器
    printf("Timer count is %d ms.\r\n", timer_get(TC_TIME2_CH0));               // 获取定时器的值并输出
    timer_clear(TC_TIME2_CH0);                                                  // 计时值使用完毕后记得清除，避免导致下次计时不从0开始
}



// Bresenham 画线算法
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int e2;

    while (1) {
        if (x0 == x1 && y0 == y1) break;
        
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

uint8 pit_00_state;     // 00中断标志位
extern uint8 threshold;
void get_threshold()
{
    // 每隔5秒重算二值化阈值
    if (pit_00_state)
    {
        threshold = otsu_threshold((uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H);
        printf("threshold = %d\r\n", threshold);
        pit_00_state = 0;                                                      // 清空周期中断触发标志位
    }
}


uint8 pit_01_state;     // 01中断标志位
extern int angel;
void get_angel()
{
    if(pit_01_state)        //20ms 编码器|电机|陀螺仪
        {
            Gyroscope_Begin(GYROSCOPE_GYRO_Z);
            Gyroscope_Conut();
            pit_01_state = 0;
        }
}






