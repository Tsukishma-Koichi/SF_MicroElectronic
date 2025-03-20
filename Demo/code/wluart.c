#include "wluart.h"

void WLUART_Init(void)
{
    if(wireless_uart_init())                                                    // �ж��Ƿ�ͨ����ʼ��
    {
        while(1)                                                                // ��ʼ��ʧ�ܾ����������ѭ��
        {
            gpio_toggle_level(LED1);                                            // ��ת LED ���������ƽ ���� LED ����
            system_delay_ms(100);                                               // ����ʱ�������Ʊ�ʾ�쳣
        }
    }
//    wireless_uart_send_byte('\r');
//    wireless_uart_send_byte('\n');
//    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // ��ʼ������ ���������Ϣ
}


/*
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��
// *************************** ����Ӳ������˵�� ***************************
// ��������ת����ģ��
//      ģ��ܽ�            ��Ƭ���ܽ�
//      RX                  �鿴 zf_device_wrieless_uart.h �� WIRELESS_UART_RX_PINx  �궨��
//      TX                  �鿴 zf_device_wrieless_uart.h �� WIRELESS_UART_TX_PINx  �궨��
//      RTS                 �鿴 zf_device_wrieless_uart.h �� WIRELESS_UART_RTS_PINx �궨��
//      GND                 ���İ��Դ�� GND
//      3V3                 ���İ� 3V3 ��Դ

// **************************** �������� ****************************
#define LED1                    (P19_0)
uint8 data_buffer[32];
uint8 data_len;


int main(void)
{
    clock_init(SYSTEM_CLOCK_250M); 	// ʱ�����ü�ϵͳ��ʼ��<��ر���>
    debug_info_init();                  // ���Դ�����Ϣ��ʼ��
    
    // �˴���д�û����� ���������ʼ�������
    uint8 count = 0;

    gpio_init(LED1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                             // ��ʼ�� LED1 ��� Ĭ�ϸߵ�ƽ �������ģʽ
    if(wireless_uart_init())                                                    // �ж��Ƿ�ͨ����ʼ��
    {
        while(1)                                                                // ��ʼ��ʧ�ܾ����������ѭ��
        {
            gpio_toggle_level(LED1);                                            // ��ת LED ���������ƽ ���� LED ����
            system_delay_ms(100);                                               // ����ʱ�������Ʊ�ʾ�쳣
        }
    }
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("SEEKFREE wireless uart demo.\r\n");              // ��ʼ������ ���������Ϣ

    // �˴���д�û����� ���������ʼ�������
    while(true)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���

        data_len = (uint8)wireless_uart_read_buffer(data_buffer, 32);             // �鿴�Ƿ�����Ϣ Ĭ�ϻ������� WIRELESS_UART_BUFFER_SIZE �ܹ� 64 �ֽ�
        if(data_len != 0)                                                       // �յ�����Ϣ ��ȡ�����᷵��ʵ�ʶ�ȡ�������ݸ���
        {
            wireless_uart_send_buffer(data_buffer, data_len);                     // ���յ�����Ϣ���ͻ�ȥ
            memset(data_buffer, 0, 32);
            func_uint_to_str((char *)data_buffer, data_len);
            wireless_uart_send_string("\r\ndata len:");                                 // ��ʾʵ���յ���������Ϣ
            wireless_uart_send_buffer(data_buffer, strlen((const char *)data_buffer));    // ��ʾ�յ������ݸ���
            wireless_uart_send_string(".\r\n");
        }
        if(count ++ > 10)
        {
            count = 0;
            gpio_toggle_level(LED1);                                            // ��ת LED ���������ƽ ���� LED ����
        }
        system_delay_ms(50);

        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

// **************************** �������� ****************************
// *************************** ���̳�������˵�� ***************************
// ��������ʱ�밴�������������б���
// ����1������û������
//      �鿴 ������� �򿪵��Ƿ�����ȷ�Ĵ��ڣ����򿪵� COM ���Ƿ��Ӧ���� ����ת����ģ�� �� COM ��
//      �鿴���İ�LED��״̬ ���������˸֤����ʼ��ʧ�� ������ ���߰���˵������ģ���Ƿ�����
// ����2��������������
//      �鿴�Ƿ�ģ�鲨���ʱ��޸Ĺ� �ָ���������
// ����3����������û���յ��ظ�
//      ���Կ����Ƿ��ȡ������ data_len �Ƿ�һֱ���� ����� ��ô����˵������ģ���Ƿ�����


*/
