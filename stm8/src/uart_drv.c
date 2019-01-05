#include "circle_buf.h"
#include "uart_drv.h"

void uart_init(unsigned long baud_rate, unsigned long f_master) {
  
  //Значение регистра BRR
  unsigned long brr;
  
  //Настраиваем TX на выход, а RX на вход
  PD_DDR_bit.DDR5 = 1;  //TX
  PD_DDR_bit.DDR6 = 0;  //RX
  
  //RX - плавающий вход
  PD_CR1_bit.C16 = 0;
  //Отключает внешние прерывания для RX
  PD_CR2_bit.C26 = 0;
  
  //Настройка скорости передачи
  brr = f_master / baud_rate;
    
  UART1_BRR2 = brr & 0x000F;
  UART1_BRR2 |= brr >> 12;
  UART1_BRR1 = (brr >> 4) & 0x00FF;
  
  //Четность отключена
  UART1_CR1_PIEN = 0;
  //Контроль четности отключен
  UART1_CR1_PCEN = 0;
  //8-битный режим
  UART1_CR1_M = 0;
  //Включить UART
  UART1_CR1_UART0 = 0;
  
  //Запретить прерывание по опустошению передающ. регистра
  UART1_CR2_TIEN = 0;
  //Запретить прерывание по завершению передачи
  UART1_CR2_TCIEN = 0;
  //Разрешить прерывание по заполнению приемного регистра
#ifdef UART_INT_ENABLE  
  UART1_CR2_RIEN = 1;
#else
  UART1_CR2_RIEN = 0;
#endif  
  //Запретить прерывание по освобождению линии
  UART1_CR2_ILIEN = 0;
  //Передатчик включить
  UART1_CR2_TEN = 1;
  //Приемник включить
  UART1_CR2_REN = 1;
  //Не посылать break-символ
  UART1_CR2_SBK = 0;
  
  //Один стоп-бит
  UART1_CR3_STOP = 0;
}

void uart_tx_byte(unsigned char data) {
    while (!UART1_SR_TXE);
    UART1_DR = data;
}

unsigned char uart_rx_byte() {
    unsigned char data;
      
    while (!UART1_SR_RXNE);
    data = UART1_DR;
    return data;
}

void uart_tx_data(unsigned char * data, unsigned char len) {
  while (len--) {
      uart_tx_byte(*data++);
  }
}

void uart_rx_data(unsigned char * data, unsigned char len) {
  while (len--) {
      *data++ = uart_rx_byte();
  }
}

#ifdef UART_INT_ENABLE  
CIRCLE_BUF_DEF(uart_buf, 32);

#pragma vector=UART1_R_RXNE_vector 
__interrupt void uart_rx_interrupt(void) {
  unsigned char data;
  
  data = UART1_DR;
#if 0
  uart_tx_byte(data);
#else
  circle_buf_push(&uart_buf, data);
#endif  
  
}
#endif

