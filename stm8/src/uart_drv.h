#ifndef _UART_H_
#define _UART_H_

#include <iostm8.h>
#include <intrinsics.h>
#include "circle_buf.h"

#define UART_INT_ENABLE

#define UART_BAUD_RATE  9600UL

#ifdef UART_INT_ENABLE
extern circle_buf_t uart_buf;
#endif

extern void uart_init(unsigned long baud_rate, unsigned long f_master);
extern void uart_tx_byte(unsigned char data);
extern unsigned char uart_rx_byte();
extern void uart_tx_data(unsigned char * data, unsigned char len);
extern void uart_rx_data(unsigned char * data, unsigned char len);

#endif