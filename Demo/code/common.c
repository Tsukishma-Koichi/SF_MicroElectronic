#include "headfile.h"
#define LED1  (P19_0)


// ˫��ͨ��
void my_ipc_callback(int receive_data) 
{
    //���ܵ����ݺ�Ĳ���
    
}


// �ܳ�ʼ��
void Main_Init(void)
{                  
    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��
    debug_init();                       // ���Դ�����Ϣ��ʼ��
    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);      // ��ʼ�� LED1 ��� Ĭ�ϸߵ�ƽ �������ģʽ
    
    // �жϳ�ʼ��
    pit_ms_init(PIT_CH0, 5000);
    // �����������ʹ��DEBUG���ڽ����շ�
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_DEBUG_UART);
    // ˫��ͨ�ų�ʼ��
    SCB_DisableDCache(); // �ر�DCashe
    ipc_communicate_init(IPC_PORT_2, my_ipc_callback);  // ��ʼ��IPCģ�� ѡ��˿�2 ��д�жϻص�����
    
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



















