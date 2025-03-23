#include "uart.h"



uint8 uart_get_data[64];                                                        // ���ڽ������ݻ�����
uint8 fifo_get_data[64];                                                        // fifo �������������

uint8  get_data = 0;                                                            // �������ݱ���
uint32 fifo_data_count = 0;                                                     // fifo ���ݸ���

fifo_struct uart_data_fifo;

void UART_Init(void)
{
    fifo_init(&uart_data_fifo, FIFO_DATA_8BIT, uart_get_data, 64);              // ��ʼ�� fifo ���ػ�����

    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);             // ��ʼ������
    uart_rx_interrupt(UART_INDEX, 1);                                           // ���� UART_INDEX �Ľ����ж�

//    uart_write_string(UART_INDEX, "UART Text.");                                // ���������Ϣ
//    uart_write_byte(UART_INDEX, '\r');                                          // ����س�
//    uart_write_byte(UART_INDEX, '\n');                                          // �������
}

uint32 UART_GetFlag(void)
{
    fifo_data_count = fifo_used(&uart_data_fifo);                           // �鿴 fifo �Ƿ�������
    if(fifo_data_count != 0)                                                // ��ȡ��������
    {
        fifo_read_buffer(&uart_data_fifo, fifo_get_data, &fifo_data_count, FIFO_READ_AND_CLEAN);    // �� fifo �����ݶ�������� fifo ���صĻ���
//        uart_write_string(UART_INDEX, "\r\nUART get data:");                // ���������Ϣ
//        uart_write_buffer(UART_INDEX, fifo_get_data, fifo_data_count);      // ����ȡ�������ݷ��ͳ�ȥ
        
    }
    return fifo_data_count;
}


//-------------------------------------------------------------------------------------------------------------------
// �������       UART_INDEX �Ľ����жϴ����� ����������� UART_INDEX ��Ӧ���жϵ���
// ����˵��       void
// ���ز���       void
// ʹ��ʾ��       uart_rx_interrupt_handler();
//-------------------------------------------------------------------------------------------------------------------
void uart_rx_interrupt_handler (void)
{
//    get_data = uart_read_byte(UART_INDEX);                                      // �������� while �ȴ�ʽ ���������ж�ʹ��
    if(uart_query_byte(UART_INDEX, &get_data))                                  // �������� ��ѯʽ �����ݻ᷵�� TRUE û�����ݻ᷵�� FALSE
    {
        fifo_write_buffer(&uart_data_fifo, &get_data, 1);                       // ������д�� fifo ��
    }
}
