#include "headfile0.h"
#define LED1  (P19_0)


// �ܳ�ʼ��
void Main_Init(void)
{                  
    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��
    debug_init();                       // ���Դ�����Ϣ��ʼ��
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);      // ��ʼ�� LED1 ��� Ĭ�ϸߵ�ƽ �������ģʽ
    
    // �жϳ�ʼ��
    pit_ms_init(PIT_CH0, 5000);
    pit_ms_init(PIT_CH1, 5);
    // �����������ʹ��DEBUG���ڽ����շ�
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    
    timer_init(TC_TIME2_CH0, TIMER_MS);                                         // ��ʱ��ʹ�� TC_TIME2_CH0 ʹ�ú��뼶����
    
    //  ips114��ʼ��
    ips114_init();
    ips114_show_string(0, 0, "mt9v03x init.");
    while(1)
    {
        if(mt9v03x_init())
            ips114_show_string(0, 16, "mt9v03x reinit.");
        else
            break;
        system_delay_ms(100);                             // ����ʱ�������Ʊ�ʾ�쳣
    }
    ips114_show_string(0, 16, "init success.");
    
    // �����ǳ�ʼ��
    imu660ra_init();
    Gyroscope_Init(GYROSCOPE_IMU660RA, 5);
    
    //  ����ͷ��ʼ��
    while(1)
    {
        if(mt9v03x_init())
            gpio_toggle_level(LED1);                                         // ��ת LED ���������ƽ ���� LED ���� ��ʼ����������ƻ����ĺ���
        else
            break;
        system_delay_ms(500);                                                   // ���Ʊ�ʾ�쳣
    }
}

void time_cnt()
{
    timer_start(TC_TIME2_CH0);                                  // ������ʱ                                             
    //...
    timer_stop(TC_TIME2_CH0);                                                   // ֹͣ��ʱ��
    printf("Timer count is %d ms.\r\n", timer_get(TC_TIME2_CH0));               // ��ȡ��ʱ����ֵ�����
    timer_clear(TC_TIME2_CH0);                                                  // ��ʱֵʹ����Ϻ�ǵ���������⵼���´μ�ʱ����0��ʼ
}



// Bresenham �����㷨
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

uint8 pit_00_state;     // 00�жϱ�־λ
extern uint8 threshold;
void get_threshold()
{
    // ÿ��5�������ֵ����ֵ
    if (pit_00_state)
    {
        threshold = otsu_threshold((uint8 *)mt9v03x_image, MT9V03X_W, MT9V03X_H);
        printf("threshold = %d\r\n", threshold);
        pit_00_state = 0;                                                      // ��������жϴ�����־λ
    }
}


uint8 pit_01_state;     // 01�жϱ�־λ
extern int angel;
void get_angel()
{
    if(pit_01_state)        //20ms ������|���|������
        {
            Gyroscope_Begin(GYROSCOPE_GYRO_Z);
            Gyroscope_Conut();
            pit_01_state = 0;
        }
}






