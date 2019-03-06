#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm8s.h>
#include <uart.h>
#include <delay.h>

#define LED_PIN         3
#define CNT_WATER_H_PID 2

void cnt_water_h() __interrupt(EXTI3_ISR) {
    PD_ODR ^= (1 << LED_PIN);
}


void uart_interrupt() __interrupt(UART1_RXC_ISR) {
    PD_ODR ^= (1 << LED_PIN);
    UART1_SR &= ~(1 << UART2_SR_RXNE);
}

void main() {
    uart_init();

    PD_DDR |= (1 << LED_PIN);
    PD_CR1 |= (1 << LED_PIN);

    { 
       PD_DDR |= (0 << CNT_WATER_H_PID);
       PD_CR1 |= (1 << CNT_WATER_H_PID);
       PD_CR2 |= (1 << CNT_WATER_H_PID);
       EXTI_CR1 |= 2 << 7;
    }

    {
	   UART1_CR2 |= (1 << UART1_CR2_RIEN);       
    }

    enable_interrupts();
    while (1) {
        delay_ms(500);
        uart_write(0x00);
        delay_ms(500);
        uart_write(0x01);        
    }
}
