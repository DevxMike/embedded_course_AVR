#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>

#include "../gpio.hpp"
#include "../timer8_t.hpp"
#include "../interrupts.hpp"
#include "../common_defs.hpp"
#include "../LEDDisplay.hpp"

static inline void init_hw_timebase(timer8_t& t) {
    *t.tccra |= (1 << WGM01);  
    *t.tccrb |= (1 << CS01) | (1 << CS00); 
    *t.ocra = 249; 
    *t.timsk |= (1 << OCIE0A); 
}

void timer0_compa_callback(timer8_t& t) {
    (void)t;
    ++millis;
}

timer8_t timer0 = timer0_base;
volatile uint32_t millis = 0;

static inline void init_hw_led_timer(timer8_t& t) {
    *t.tccra |= (1 << WGM21);  
    *t.tccrb |= (1 << CS22);
    *t.ocra = 249; 
    *t.timsk |= (1 << OCIE2A); 
}

GPIO_t GPIOD_desc = GPIOx_t(D);
Digital_IO segment_pins[]{ 
    { GPIOD_desc, PD7 }, { GPIOD_desc, PD6 },
    { GPIOD_desc, PD5 }, { GPIOD_desc, PD4 }, { GPIOD_desc, PD3 }, 
    { GPIOD_desc, PD2 }, { GPIOD_desc, PD1 }, { GPIOD_desc, PD0 }
};

GPIO_t GPIOC_desc = GPIOx_t(C);
Digital_IO common_pins[]{
    { GPIOC_desc, PC0 }, { GPIOC_desc, PC1 },
    { GPIOC_desc, PC2 }, { GPIOC_desc, PC3 }
};

LEDDisplay<4> my_display(common_pins, segment_pins);
uint32_t value_timer;

timer8_t timer2 = timer2_base;

void timer2_compa_callback(timer8_t& t) {
    (void)t;
    my_display.next();
}

int main() {
    for(uint8_t i = 0; i < sizeof(segment_pins) / sizeof(Digital_IO); ++i) {
        segment_pins[i].init(Digital_IO::OUTPUT);
    }

    for(uint8_t i = 0; i < sizeof(common_pins) / sizeof(Digital_IO); ++i){
        common_pins[i].init(Digital_IO::OUTPUT);
    }

    timer0.compareA_cb = timer0_compa_callback;
    timer2.compareA_cb = timer2_compa_callback;

    init_hw_timebase(timer0);
    init_hw_led_timer(timer2);

    sei();

    SignleLEDSegment segments[4] {
        { .value = 0, .dot_active = false },
        { .value = 1, .dot_active = false },
        { .value = 2, .dot_active = false },
        { .value = 3, .dot_active = false }
    };

    static uint32_t tmp = 0;

    while(1) {
        if((get_timestamp() > value_timer) && (get_timestamp() - value_timer >= 1000)) {
            tmp++;
            segments[0].value = (tmp / 1000) % 10;
            segments[1].value = (tmp / 100) % 10;
            segments[2].value = (tmp / 10) % 10;
            segments[3].value = tmp % 10;

            segments[1].dot_active = !segments[1].dot_active;
            
            my_display.set_segments(segments);

            value_timer += 1000;
        }
    }
}