#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include "../common/include/main.hpp"


timer8_t timer0 = timer0_base;
     
static inline void init_hw_uart(UART_t& iface, uint16_t baudrate) {
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    *iface.ubrrh = (ubrr >> 8) & 0xFF; 
    *iface.ubrrl = ubrr & 0xFF;

    *iface.ucsrc = (1 << UCSZ01) | (1 << UCSZ00); 
    *iface.ucsrb |= (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0); 
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

UART_t usart0 = usart_base;
Communication<UART_t> uart_handle(usart0);

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;

    sei();

    while(1) {
        if (Timebase::now() - uart_timer >= 200) {
            uart_handle.flush();
            uart_timer = Timebase::now();
        }

        if (Timebase::now() - delay_timer >= 1000) {
            uart_handle.puts("Hello world!\n\r");
            delay_timer = Timebase::now();
        }
    }
}