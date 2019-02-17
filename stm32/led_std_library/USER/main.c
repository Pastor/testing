#include <stdbool.h>
#include <stdint.h>
#include "stm32f10x.h"
#include "led.h"

static void delay_ms(uint32_t ms);

int main(void) {	
	LED_GPIO_Config();

	while (true) {
		delay_ms(500);
		LED1( ON );			  
		delay_ms(500);
		LED1( OFF );
	}
}

void delay_ms(uint32_t ms) {
	uint32_t c = (ms * (SystemCoreClock / 10000) );	
	for(; c != 0; c--);
}
