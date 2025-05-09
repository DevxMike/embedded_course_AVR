#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>

#include "../gpio.hpp"
// #include "../communication.hpp"
#include "../timer8_t.hpp"
#include "../interrupts.hpp"
#include "../common_defs.hpp"
#include "../button.hpp"

static inline void init_hw_timebase(timer8_t& t) {
    *t.tccra |= (1 << WGM01);  
    *t.tccrb |= (1 << CS01) | (1 << CS00); 
    *t.ocra = 249; 
    *t.timsk |= (1 << OCIE0A); 
}

static inline void init_hw_uart(UART_t& iface, uint16_t baudrate) {
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    *iface.ubrrh = (ubrr >> 8) & 0xFF; 
    *iface.ubrrl = ubrr & 0xFF;

    *iface.ucsrc = (1 << UCSZ01) | (1 << UCSZ00); 
    *iface.ucsrb |= (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0); 
}

void timer0_compa_callback(timer8_t& t) {
    (void)t;
    ++millis;
}

void rx_callback(UART_t& iface) {
    iface.rx_buffer.push(*iface.udr);
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

timer8_t timer0 = timer0_base;
volatile uint32_t millis = 0;

UART_t usart0 = usart_base;
UART_Comm uart_handle(usart0);

GPIO_t GPIOB = GPIOx_t(B);

Digital_IO led_output { GPIOB, PB5 };
Digital_IO button_input { GPIOB, PB0 };

void button_callback(Digital_IO& io) {
    static bool led_state = false;
    (void)io;

    led_state = !led_state;
    led_output.set_output(led_state);
}

PushButton button_manager { 
    button_input,
    get_timestamp,
    PushButton::callback_set(button_callback)
};

uint32_t poll_timer;
uint32_t log_timer;


int main() {
    timer0.compareA_cb = timer0_compa_callback;

    init_hw_timebase(timer0);

    sei();

    led_output.init(Digital_IO::OUTPUT);
    button_input.init(Digital_IO::INPUT_PULLUP);

    while(1) {
        if((get_timestamp() > poll_timer) && (get_timestamp() - poll_timer >= 10)) {
            button_manager.poll();
            poll_timer += 10;
        }
    }
}