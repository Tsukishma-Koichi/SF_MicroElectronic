#ifndef _UART_H_
#define _UART_H_

#include "zf_common_headfile.h"

#define UART_INDEX              (DEBUG_UART_INDEX   )                           // Ĭ�� UART_0
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // Ĭ�� 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // Ĭ�� UART0_TX_P00_1
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // Ĭ�� UART0_RX_P00_0

extern uint8 uart_get_data[];                                                        // ���ڽ������ݻ�����
extern uint8 fifo_get_data[];                                                        // fifo �������������

extern uint8  get_data;                                                            // �������ݱ���
extern uint32 fifo_data_count;                                                     // fifo ���ݸ���

extern fifo_struct uart_data_fifo;

void UART_Init(void);
uint32 UART_GetFlag(void);

#endif