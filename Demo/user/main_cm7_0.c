#include "zf_common_headfile.h"
#include "headfile0.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������


// **************************** �������� ****************************



#define PIT0                             (PIT_CH0 )                             // ʹ�õ������жϱ��
#define PIT1                             (PIT_CH1 )
#define PIT2                             (PIT_CH2 )

#define KEY1                    (P20_0)
#define KEY2                    (P20_1)
#define KEY3                    (P20_2)
uint8 pit_00_state = 0;
uint8 pit_01_state = 0;
uint8 pit_02_state = 0;

uint8 key2_state = 0;

#define FLASH_SECTION_INDEX       (0)                                 // �洢�����õ�����
#define FLASH_PAGE_INDEX          (0)                                // �洢�����õ�ҳ�� ������һ��ҳ��
uint16 i = 0;

#define NAV_LEN     (300)

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��<��ر���>
    debug_init();                   // ���Դ�����Ϣ��ʼ��
    OSC_Init();
    pit_ms_init(PIT0, 20);
    pit_ms_init(PIT1, 100);
    pit_ms_init(PIT2, 20);
//    WLUART_Init();
    UART_Init();
    Encoder_Init();
    Motor_Init();
    flash_init();                                                               // ʹ��flashǰ�ȵ���flash��ʼ��
    flash_buffer_clear();                                                       // ��ջ�����
    
//    SCB_DisableDCache(); // �ر�DCashe
//    ipc_communicate_init(IPC_PORT_1, my_ipc_callback);  // ��ʼ��IPCģ�� ѡ��˿�1 ��д�жϻص�����

    //imu660ra_init();
   // Gyroscope_Init(GYROSCOPE_IMU660RA, 20); // ��ʼ�������ǣ�����ʱ����Ϊ 5ms
    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);               // ��ʼ�� KEY1 ���� Ĭ�ϸߵ�ƽ ��������
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);

    // PID��ʼ��
    Motor_PID_Init();  
    
    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAV_LEN);        // �����ݴ� flash ��ȡ��������
    for (uint16 i=0; i < NAV_LEN; i++)
    {
        printf("\r\n %d: %d", i, flash_union_buffer[i].int16_type);
    }
    printf("over2");
    
    while(true)         //һ������
    {
        if(!gpio_get_level(KEY1))         // ��ȡ KEYx ��ƽΪ��
        {
            break;
        }
    }

    while(true)
    {
        SCB_CleanInvalidateDCache_by_Addr(&m7_0_data, sizeof(m7_0_data));


        if(pit_00_state)        //20ms ������|���|������
        {
//            Gyroscope_Begin(GYROSCOPE_GYRO_Z);
//            Gyroscope_Conut();
            //Gyroscope_GetData();
            Encoder_SpeedRead();
            Monitor_ReRead();
            
            Motor_Diff(Monitor_Data);
            
            Motor1_PIDwork();
            Motor2_PIDwork();
            
//            Motor_SetSpeed(MOTOR_2, 3000);
//            Motor_SetSpeed(MOTOR_1, 3000);
            pit_00_state = 0;
        }
        
        if(pit_01_state)        //100ms ����ʾ����
        {
//            OSC_Send((int)Gyro_z, Monitor_Data, Motor1_target, Motor2_target, Motor_target, Encoder_1Data, Encoder_2Data, filtered_z_gyro);

            pit_01_state = 0;
        }
        
        if(pit_02_state)        //10ms
        {
//            if (i < NAV_LEN)
//            {
//                flash_union_buffer[i].int16_type = Monitor_Data;
//                printf("%d:%d\t%d\n", i, flash_union_buffer[i].int16_type, Monitor_Data);
//                i++;
//            }
            
            pit_02_state = 0;
        }
        
//        if (i == NAV_LEN)
//        {
//            break;
//        }
//        system_delay_ms(100);
    }
    
//    flash_write_page_from_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAV_LEN);     // ��ָ�� Flash ������ҳ��д�뻺��������
//    printf("over1");
    
//    flash_read_page_to_buffer(FLASH_SECTION_INDEX, FLASH_PAGE_INDEX, NAL_LEN);        // �����ݴ� flash ��ȡ��������
//    for (uint16 i=0; i < NAV_LEN; i++)
//    {
//        printf("\r\n %d: %d", i, flash_union_buffer[i].int16_type);
//    }
//    printf("over2");
    
}


// ʹ�� cyt4bb7 ���İ�ֱ�ӽ��߽��в���
//      ģ��ܽ�            ��Ƭ���ܽ�
//      1DIR                P09_1 ���� P05_1
//      1PWM                P09_0 ���� P05_0
//      GND                 GND
//      2DIR                P10_3 ���� P05_3
//      2PWM                P10_2 ���� P05_2
//      GND                 GND
//      ���߶��� +          �������
//      ���߶��� -          ��ظ���
// ʹ�� cyt4bb7 ѧϰ������в���
//      ��ģ��ĵ�Դ���߶�������������������������
//      ��ģ����źŽӿ�ʹ�����׻��������������źŽӿ����� ��ע����߷��� ��ȷ��������������ñ�ȷ��һ�� ���Ųο��Ϸ����İ�����
//      �������빩������ȷ����
// *************************** ���̳�������˵�� ***************************
// ��������ʱ�밴�������������б���
// ����1�������ת����ģ�������ѹ�ޱ仯
//      ���ʹ��������ԣ��������Ҫ�õ�ع���
//      ���ģ���Ƿ���ȷ���ӹ��� ����ʹ�õ�Դ�߹��� ����ʹ�öŰ���
//      �鿴�����Ƿ�������¼���Ƿ����ر���ȷ���������¸�λ����
//      ���ñ������Ӧ PWM ���ŵ�ѹ�Ƿ�仯��������仯֤������δ���У����������𻵣����߽Ӵ����� ��ϵ�����ͷ�