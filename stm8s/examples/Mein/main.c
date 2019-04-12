#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stm8s.h>
#include <i2c.h>
#include <uart.h>
#include <delay.h>

#define MEMORY_ADDR        0xA0
#define VALUE1_ADDR        0x0000

/*
EXTI0 - A
EXTI1 - B
EXTI2 - C
EXTI3 - D
EXTI4 - E
*/

#define LED_PIN   3
#define CNT_H_PID 3
#define CNT_C_PID 4
#define is_h() (PC_IDR & (1 << CNT_H_PID))
#define is_c() (PC_IDR & (1 << CNT_C_PID))
#define toggle_led() (PD_ODR ^= (1 << LED_PIN))

uint32_t read_uint32(uint16_t address);
void write_uint32(uint16_t address, uint32_t value);

#pragma codeseg ram_seg
volatile uint32_t h_count = 0;
bool h_updated = false;
bool c_updated = false;

#pragma codeseg ram_seg
void cnt_water_h() __interrupt(EXTI2_ISR) {
    uint32_t m_count = read_uint32(VALUE1_ADDR);
    /*toggle_led();*/
    h_updated = true;
    if ( is_h() ) {
    	h_count++;
    }
    write_uint32(VALUE1_ADDR, m_count + 1);
    /*c_updated = true;*/

}

uint32_t read_uint32(uint16_t address) {
    uint32_t r = 0;

    i2c_start();
    i2c_write_addr((uint8_t)(MEMORY_ADDR + I2C_WRITE));
    i2c_write(address >> 8);
    i2c_write(address & 0xFF);
    i2c_stop();

    i2c_start();
    i2c_write_addr((uint8_t)(MEMORY_ADDR + I2C_READ));
    i2c_read_arr(&r, sizeof(uint32_t));
    delay_ms(6);
    return r;
}

static inline void write_byte(uint16_t address, uint8_t value) {
    i2c_start();
    i2c_write_addr((uint8_t)(MEMORY_ADDR + I2C_WRITE));
    i2c_write(address >> 8);
    i2c_write(address & 0xFF);
    i2c_write(value);
    i2c_stop();
    delay_ms(6);
}


void write_uint32(uint16_t address, uint32_t value) {
    write_byte( address + 0, ((uint8_t *)&value)[0] );
    write_byte( address + 1, ((uint8_t *)&value)[1] );
    write_byte( address + 2, ((uint8_t *)&value)[2] );
    write_byte( address + 3, ((uint8_t *)&value)[3] );
}


int putchar(int c) {
    uart_write(c);
    return 0;
}

int getchar() {
    return uart_read();
}


void main() {
    uint16_t  i_count = 0;
    
    disable_interrupts();
    PD_DDR |= (1 << LED_PIN);
    PD_CR1 |= (1 << LED_PIN);
    uart_init();
    i2c_init();
    {
       printf("\nInitialize interrupts\r\n"); 
       PC_DDR &= ~(1 << CNT_H_PID); /* PD_DDR &= ~(1 << CNT_H_PID); */
       PC_CR1 |= (1 << CNT_H_PID);
       PC_CR2 |= (1 << CNT_H_PID);
       EXTI_CR1 |= 0 << 5;
    }
    
    toggle_led();
    enable_interrupts();
    write_uint32(VALUE1_ADDR, 0x000000000);
    while (true) {
        __asm__("halt");

        if (h_updated == true) {
            uint32_t v = read_uint32(VALUE1_ADDR);
            printf("Int.H[%05d]: %02d%02d%02d%02d, %02d%02d%02d%02d\r\n", 
                i_count++, ((uint8_t *)&v)[0], ((uint8_t *)&v)[1], ((uint8_t *)&v)[2], ((uint8_t *)&v)[3],
                           ((uint8_t *)&h_count)[0], ((uint8_t *)&h_count)[1], ((uint8_t *)&h_count)[2], ((uint8_t *)&h_count)[3]);
            /*toggle_led();*/
            h_updated = false;
        }
        if (c_updated == true) {
            printf("Int.C[%05d]\r\n", i_count++);
            /*toggle_led();*/
            c_updated = false;
        }          
        /*delay_ms(500);*/
    }
}
