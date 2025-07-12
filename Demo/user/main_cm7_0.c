#include "zf_common_headfile.h"
#include "headfile0.h"

// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完


// **************************** 代码区域 ****************************



#define PIT0                             (PIT_CH0 )                             // 使用的周期中断编号
#define PIT1                             (PIT_CH1 )
#define PIT2                             (PIT_CH2 )

#define KEY1                    (P20_0)
#define KEY2                    (P20_1)
#define KEY3                    (P20_2)
#define BUZZER_PIN              (P19_4)
uint8 pit_00_state = 0;
uint8 pit_01_state = 0;
uint8 pit_02_state = 0;

uint8 key2_state = 0;

#define FLASH_SECTION_INDEX       (0)                                 // 存储数据用的扇区
#define FLASH_PAGE_INDEX          (0)                                // 存储数据用的页码 倒数第一个页码
uint16 Cnt = 43;

#define NAV_LEN     (150)
int16 NAV[NAV_LEN];

int Last_SSZ, Last_OR;
uint8 Nav_Flag = 0;

uint8 Bend_Flag = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// 时钟配置及系统初始化<务必保留>
    debug_init();                   // 调试串口信息初始化
    OSC_Init();
    pit_ms_init(PIT0, 20);
    pit_ms_init(PIT1, 100);
    pit_ms_init(PIT2, 20);
//    WLUART_Init();
    UART_Init();
    Encoder_Init();
    Motor_Init();
    flash_init();                                                               // 使用flash前先调用flash初始化
    flash_buffer_clear();                                                       // 清空缓冲区
    gpio_init(BUZZER_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    
//    SCB_DisableDCache(); // 关闭DCashe
//    ipc_communicate_init(IPC_PORT_1, my_ipc_callback);  // 初始化IPC模块 选择端口1 填写中断回调函数

    //imu660ra_init();
   // Gyroscope_Init(GYROSCOPE_IMU660RA, 20); // 初始化陀螺仪，设置时间间隔为 5ms
    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);               // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);

    // PID初始化
    Motor_PID_Init();  
        
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAV_LEN);        // 将数据从 flash 读取到缓冲区
    for (uint16 i=Cnt; i < NAV_LEN; i++)
    {
        NAV[i] = flash_union_buffer[i].int16_type;
        printf("\r\n %d: %d", i, flash_union_buffer[i].int16_type);
    }
//    printf("over2");
    
    while(true)         //一键启动
    {
        if(!gpio_get_level(KEY1))         // 获取 KEYx 电平为低
        {
            break;
        }
    }
    
    m7_0_data[4] = 1;
    SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
    system_delay_ms(100);

    while(true)
    {
        SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));
        
        if (Last_SSZ == 0 && m7_0_data[1] == 3)    //停
        {
            Motor_SetSpeed(MOTOR_2, 0);
            Motor_SetSpeed(MOTOR_1, 0);
            break;
        }
        Last_SSZ = m7_0_data[1];

        if(pit_00_state)        //20ms 编码器|电机|陀螺仪
        {
//            Gyroscope_Begin(GYROSCOPE_GYRO_Z);
//            Gyroscope_Conut();
            //Gyroscope_GetData();
            Encoder_SpeedRead();
            
            
            if (Last_OR == 0 && m7_0_data[1] == 4)
            {
                Nav_Flag = 1;
//                printf("%d", m7_0_data[1]);
            }
            Last_OR = m7_0_data[1];
            
            if (Bend_Flag == 0 && m7_0_data[1] == 2 && m7_0_data[2] == 1)
            {
                Bend_Flag = 1;
            }
            else if (Bend_Flag == 0 && m7_0_data[1] == 2 && m7_0_data[2] == 2)
            {
                Bend_Flag = 2;
            }
          
            
            if (Nav_Flag == 1 && Cnt < NAV_LEN)
            {
                gpio_toggle_level(BUZZER_PIN);
                Motor_Diff(NAV[Cnt]);
                Cnt++;
                if (Cnt == NAV_LEN)
                {
                  gpio_set_level(BUZZER_PIN, GPIO_LOW);
                  Nav_Flag = 0;
                  printf("\r\n Nav_Flag:%d", Nav_Flag);
                }
            }
            else if (Bend_Flag == 1)
            {
                while (1)
                {
                    Motor_Diff();
                    if (Monitor_Data )
                      break;
                }
            }
            else
            {
                Monitor_ReRead();
                Motor_Diff(Monitor_Data);
            }
//            if (Cnt < NAV_LEN)
//            {
//                flash_union_buffer[Cnt].int16_type = Monitor_Data;
//                printf("%d:%d\t%d\n", Cnt, flash_union_buffer[Cnt].int16_type, Monitor_Data);
//                Cnt++;
//            }
            
//            Motor_Diff(NAV[Cnt]);
//            printf("\r\nNAV:%d", NAV[Cnt]);
//            Cnt++;
//            if (Cnt == NAV_LEN)
//            {
//                break;
//            }
            
            Motor1_PIDwork();
            Motor2_PIDwork();
            
//            Motor_SetSpeed(MOTOR_2, 3000);
//            Motor_SetSpeed(MOTOR_1, 3000);
            pit_00_state = 0;
        }
//        if (Cnt == NAV_LEN)
//        {
//            break;
//        }
        
        if(pit_01_state)        //100ms 虚拟示波器
        {
//            OSC_Send((int)Gyro_z, Monitor_Data, Motor1_target, Motor2_target, Motor_target, Encoder_1Data, Encoder_2Data, filtered_z_gyro);

            pit_01_state = 0;
        }
        
        if(pit_02_state)        //10ms
        {

            
            pit_02_state = 0;
        }
        

//        system_delay_ms(100);
    }
    
//    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAV_LEN);     // 向指定 Flash 扇区的页码写入缓冲区数据
//    printf("over1");
//    
//    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAV_LEN);        // 将数据从 flash 读取到缓冲区
//    for (uint16 i=0; i < 50; i++)
//    {
//        printf("\r\n %d: %d", i, flash_union_buffer[i].int16_type);
//    }
//    printf("over2");
    Motor_SetSpeed(MOTOR_2, 0);
    Motor_SetSpeed(MOTOR_1, 0);
    
}


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