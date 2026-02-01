#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include <avr/interrupt.h>

#include "../gpio.hpp"

static inline void init_hw_timebase() {
    TCCR0A |= (1 << WGM01); // tryb CTC 
    TCCR0B |= (1 << CS01) | (1 << CS00); // 64 preskaler
    OCR0A = 249; // przerwanie co 1kHz (1ms)
    TIMSK0 |= (1 << OCIE0A); // wlacz przerwanie compare match
}

volatile uint32_t millis;

ISR(TIMER0_COMPA_vect) {
    ++millis;
}


int main() {
    millis = 0;

    init_hw_timebase();
    sei();

    GPIO_t GPIOB {
        .ddr_reg = &DDRB,
        .pin_reg = &PINB,
        .port_reg = &PORTB
    };

    Digital_IO led { GPIOB, PB5 };
    led.init(Digital_IO::OUTPUT);

    static uint32_t led_timer;
    bool led_state = false;

    while(1) {
        if((led_timer < millis) && (millis - led_timer >= 300)) {
            led_state = !led_state;
            led_timer += 300;

            led.set_output(led_state);
        }
    }
}