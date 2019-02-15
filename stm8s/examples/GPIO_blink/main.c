#include <stdint.h>
#include <stm8s.h>
#include <delay.h>

#define LED_PIN 3
#define RF_PIN  2

void main() {
    PD_DDR |= (1 << LED_PIN);
    PD_CR1 |= (1 << LED_PIN);

    while (1) {
        PD_ODR ^= (1 << LED_PIN);
	PD_ODR ^= (1 << RF_PIN);
        delay_ms(500);
    }
}
