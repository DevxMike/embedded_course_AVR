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
Digital_IO led { GPIOB, PB5 };

static void UART_rx_LED_ctl(UART_Comm& iface, Digital_IO& led) {
    auto next = iface.getc();

    enum {
        TURN_OFF = 'f',
        TURN_ON = 'o'
    } availableCommands;

    if(next) {
        char c = next.get();

        switch(c) {
            case TURN_OFF:
            case TURN_ON:
                iface.puts("CMD OK, LED: ");
                break;
            
            default:
                iface.puts("CMD unknown\n\r");
                break;
        }

        switch(c) {
            case TURN_OFF:
                led.set_output(false);
                iface.puts("OFF\n\r");
                break;

            case TURN_ON:
                led.set_output(true);
                iface.puts("ON\n\r");
                break;
        }
    }
}

int main() {
    timer0.compareA_cb = timer0_compa_callback;
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_timebase(timer0);
    init_hw_uart(usart0, 9600);

    sei();
    
    led.init(Digital_IO::OUTPUT);

    static uint32_t uart_timer;

    while(1) {
        uint32_t now = millis;

        if((uart_timer < now) && (now - uart_timer > 10)) {
            uart_handle.flush();
            UART_rx_LED_ctl(uart_handle, led);

            uart_timer += 5;
        }
    }
}