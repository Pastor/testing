#include "uart_drv.h"

#define EEPROM_BASE_ADDRESS 0x1000

#pragma location = EEPROM_BASE_ADDRESS
__no_init unsigned long F_MASTER_E;
/*__no_init unsigned long F_MASTER_E = 2000000UL;*/

#define F_CPU  2000000UL

unsigned int clock(void) {
    unsigned char h = TIM1_CNTRH;
    unsigned char l = TIM1_CNTRL;
    return((unsigned int)(h) << 8 | l);
}

inline void 
enable_eeprom_write() {
  /** FLASH_IAPSR_DUL = 0; - write protect */
  if ( FLASH_IAPSR_DUL == 0 ) {
    FLASH_DUKR = 0xAE;
    FLASH_DUKR = 0x56; 
  }
}

inline void 
enable_flash_write() {
  FLASH_PUKR = 0x56;
  FLASH_PUKR = 0xAE; 
}

static inline void delay_ms(unsigned short ms) {
    unsigned int i;
    for (i = 0; i < ((F_CPU / 18000UL) * ms); i++)
        __no_operation();
}

int 
main(void) {
#ifndef UART_INT_ENABLE
    unsigned char data;
#endif

    uart_init(UART_BAUD_RATE, F_CPU);
    uart_tx_data("Hello, world!\r\n", 15);

#ifdef UART_INT_ENABLE
    __enable_interrupt();
#endif

    while (1) {
#ifndef UART_INT_ENABLE
        uart_rx_data(&data, 1);
        uart_tx_data(&data, 1);
#else
        unsigned char it;

        while (circle_buf_pop(&uart_buf, &it) == 0) {
            uart_tx_byte(it);
        }
#endif
    }
}
