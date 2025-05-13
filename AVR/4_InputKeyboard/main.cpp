#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>

#include "../gpio.hpp"
#include "../communication.hpp"
#include "../timer8_t.hpp"
#include "../interrupts.hpp"
#include "../common_defs.hpp"
#include "../button.hpp"
#include "../matrix_kbd.hpp"

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

timer8_t timer2 = timer2_base;

static inline void init_hw_uart(UART_t& iface, uint16_t baudrate) {
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    *iface.ubrrh = (ubrr >> 8) & 0xFF; 
    *iface.ubrrl = ubrr & 0xFF;

    *iface.ucsrc = (1 << UCSZ01) | (1 << UCSZ00); 
    *iface.ucsrb |= (1 << TXCIE0) | (1 << TXEN0); 
}

void USART0_flush(UART_t& iface) {
    auto next = iface.tx_buffer.pop();

    if(next) {
        *iface.udr = next.get();
    } 
}

void tx_callback(UART_t& iface) {
    auto next = iface.tx_buffer.pop();
    
    if(next) {
        *iface.udr = next.get();
    }
    else {
        iface.busy = false;
    }
}

UART_t usart0 = usart_base;
UART_Comm uart_handle(usart0);

GPIO_t GPIOD_desc = GPIOx_t(D);
GPIO_t GPIOB_desc = GPIOx_t(B);

Digital_IO columns[] {
    { GPIOD_desc, PD7 }, { GPIOD_desc, PD6 },
    { GPIOD_desc, PD5 }, { GPIOD_desc, PD4 }
};

Digital_IO rows[] {
    { GPIOD_desc, PD3 }, { GPIOD_desc, PD2 },
    { GPIOB_desc, PB1 }, { GPIOB_desc, PB0 }
};

MatrixKBD<4, 4> matrix4x4 { columns, rows };


void timer2_compa_callback(timer8_t& t) {
    (void)t;
    matrix4x4.poll();
}

void print_kbd() {
    uart_handle.puts("KBD STATE\n\r");

    for(uint8_t i = 0; i < 4; ++i) {
        for(uint8_t j = 0; j < 4; ++j) {
            auto reading = matrix4x4.get_button_reading(i, j);
            if(reading) {
                bool v = reading.get();

                uart_handle.puts(v? "1 " : "0 ");
            }
        }
        uart_handle.puts("\n\r");
    }
    uart_handle.puts("\n\r");
}

uint32_t log_timer;
uint32_t flush_timer;



int main() {
    timer0.compareA_cb = timer0_compa_callback;
    timer2.compareA_cb = timer2_compa_callback;
    usart0.tx_complete_cback = tx_callback;
    usart0.flush_tx = USART0_flush;

    init_hw_timebase(timer0);
    init_hw_led_timer(timer2);
    init_hw_uart(usart0, 9600);

    matrix4x4.begin();

    sei();


    while(1) {
        if((get_timestamp() > log_timer) && (get_timestamp() - log_timer >= 500)) {
            print_kbd();
            log_timer += 500;
        }
        if((get_timestamp() > flush_timer) && (get_timestamp() - flush_timer >= 5)) {
            uart_handle.flush();
            flush_timer += 5;
        }
    }
}