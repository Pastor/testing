#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stm8s.h>
#include <uart.h>
#include <delay.h>


/*
EXTI0 - A
EXTI1 - B
EXTI2 - C
EXTI3 - D
EXTI4 - E
*/

#define LED_PIN 3
#define CNT_WATER_H_PID 3
#define is_pull() (PC_IDR & (1 << CNT_WATER_H_PID))

volatile uint32_t h_count = 0;
volatile bool b_updated = false;

static inline void toggle_led() {
    PD_ODR ^= (1 << LED_PIN);
}

static void cnt_water_h() __interrupt(EXTI2_ISR) {
    /*toggle_led();*/
    b_updated = true;
    ++h_count;
}

int putchar(int c) {
    uart_write(c);
    return 0;
}

int getchar() {
    return uart_read();
}


void main() {
    uint8_t  i_count = 0;
    
    disable_interrupts();
    PD_DDR |= (1 << LED_PIN);
    PD_CR1 |= (1 << LED_PIN);
    uart_init();
    {
       printf("\nInitialize interrupts\r\n"); 
       PC_DDR &= ~(1 << CNT_WATER_H_PID); /* PD_DDR &= ~(1 << CNT_WATER_H_PID); */
       PC_CR1 |= (1 << CNT_WATER_H_PID);
       PC_CR2 |= (1 << CNT_WATER_H_PID);
       EXTI_CR1 |= 0 << 5;
    }
    
    enable_interrupts();
    while (true) {
        /*__asm
         	halt;
        __endasm;*/

        if (b_updated == true) {
        	printf("Interrupted[%05d] %d\r\n", i_count++, h_count);
        	toggle_led();
        	b_updated = false;
        }          
		/*delay_ms(500);*/
    }
}
