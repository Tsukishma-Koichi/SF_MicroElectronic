#include "zf_common_headfile.h"
#include "headfile.h"

// ���µĹ��̻��߹����ƶ���λ�����ִ�����²���
// ��һ�� �ر��������д򿪵��ļ�
// �ڶ��� project->clean  �ȴ��·�����������


// **************************** �������� ****************************
#define PIT0                             (PIT_CH0 )                             // ʹ�õ������жϱ��
#define PIT1                             (PIT_CH1 )
#define PIT2                             (PIT_CH2 )
uint8 pit_00_state = 0;
uint8 pit_01_state = 0;
uint8 pit_02_state = 0;

int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��<��ر���>
    debug_init();                  // ���Դ�����Ϣ��ʼ��
    OSC_Init();
    pit_ms_init(PIT0, 5);
    pit_ms_init(PIT1, 100);
    pit_ms_init(PIT2, 5000);
    WLUART_Init();
    Encoder_Init();
    Motor_Init();
    UART_Init();
    imu660ra_init();
    Gyroscope_Init(GYROSCOPE_IMU660RA, 5); // ��ʼ�������ǣ�����ʱ����Ϊ 10ms

    
    // PID��ʼ��
    Motor_PID_Init();

    Motor1_PID_Set(MOTOR1_PID_P, MOTOR1_PID_I, MOTOR1_PID_D, MOTOR1_PID_SL, MOTOR1_PID_UL, 1);
    Motor2_PID_Set(MOTOR2_PID_P, MOTOR2_PID_I, MOTOR2_PID_D, MOTOR2_PID_SL, MOTOR2_PID_UL, 1);

    


    while(true)
    {
     
        Motor1_target = Motor_target;
        Motor2_target = Motor_target;
        if(pit_00_state)
        {
//            printf("GX: %.2f GY: %.2f GZ: %.2f AX: %.2f AY: %.2f AZ: %.2f\r\n",Gyro_corrX, Gyro_corrY, Gyro_corrZ, Acc_corrX, Acc_corrY, Acc_corrZ);// �������ݵ�����

            Encoder_SpeedRead();
//            Motor_SetSpeed(MOTOR_2, (int16)2000);
//            Motor1_PIDwork();
//            Motor2_PIDwork();
            pit_00_state = 0;
        }
        
        if(pit_01_state)
        {
//            OSC_Send((int16)Motor_target, Encoder_1Data, Encoder_2Data);
//            
            pit_01_state = 0;
        }
        
        if(pit_02_state)
        {
//            Motor_target = 200;
            
            pit_01_state = 0;
        }
        
//        system_delay_ms(50);
      

    }
}

// **************************** �������� ****************************
// *************************** ����Ӳ������˵�� ***************************
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