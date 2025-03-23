#ifndef _UART_H_
#define _UART_H_

#include "zf_common_headfile.h"

#define UART_INDEX              (DEBUG_UART_INDEX   )                           // 默认 UART_0
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // 默认 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // 默认 UART0_TX_P00_1
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // 默认 UART0_RX_P00_0

extern uint8 uart_get_data[];                                                        // 串口接收数据缓冲区
extern uint8 fifo_get_data[];                                                        // fifo 输出读出缓冲区

extern uint8  get_data;                                                            // 接收数据变量
extern uint32 fifo_data_count;                                                     // fifo 数据个数

extern fifo_struct uart_data_fifo;

void UART_Init(void);
uint32 UART_GetFlag(void);

#endif