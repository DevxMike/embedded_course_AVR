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

static inline void init_hw_timebase(timer8_t& t) {
    *t.tccra |= (1 << WGM01); // tryb CTC 
    *t.tccrb |= (1 << CS01) | (1 << CS00); // 64 preskaler
    *t.ocra = 249; // przerwanie co 1kHz (1ms)
    *t.timsk |= (1 << OCIE0A); // wlacz przerwanie compare match
}

static inline void init_hw_uart(UART_t& iface, uint16_t baudrate) {
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    *iface.ubrrh = (ubrr >> 8) & 0xFF; // ustawienie baudrate
    *iface.ubrrl = ubrr & 0xFF;

    *iface.ucsrc = (1 << UCSZ01) | (1 << UCSZ00); // 8bit, bez parzystosci, 1 stop bit
    *iface.ucsrb |= (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0); // wlaczenie tx i rx, zezwolenie na przerwanie rx 
}

void timer0_compa_callback(timer8_t& t) {
    (void)t;
    ++millis;
}

void rx_callback(UART_t& iface) {
    iface.rx_buffer.push(*iface.udr);
}

void tx_callback(UART_t& iface) {
    if(iface.tx_buffer.empty()) {
        iface.busy = false;
    }
    else {
        *iface.udr = iface.tx_buffer.pop().get();
    }
}

void USART0_flush(UART_t& iface) {
    *iface.udr = iface.tx_buffer.pop().get();
}

UART_t usart0 {
    .udr = &UDR0,
    .ucsra = &UCSR0A,
    .ucsrb = &UCSR0B,
    .ucsrc = &UCSR0C,
    .ubrrl = &UBRR0L,
    .ubrrh = &UBRR0H,
    .rx_complete_cback = rx_callback,
    .tx_complete_cback = tx_callback,
    .UDRIE_cback = nullptr,
    .flush_tx = USART0_flush,
    .busy = false
};

timer8_t timer0 {
    .tccra = &TCCR0A,
    .tccrb = &TCCR0B,
    .tcnt = &TCNT0,
    .ocra = &OCR0A,
    .ocrb = &OCR0B,
    .timsk = &TIMSK0,
    .tifr = &TIFR0,
    .compareB_cb = nullptr,
    .compareA_cb = timer0_compa_callback,
    .timer_overflow_cb = nullptr
};

volatile uint32_t millis = 0;

int main() {
    millis = 0;

    init_hw_timebase(timer0);
    init_hw_uart(usart0, 9600);

    sei();

    GPIO_t GPIOB {
        .ddr_reg = &DDRB,
        .pin_reg = &PINB,
        .port_reg = &PORTB
    };

    Digital_IO led { GPIOB, PB5 };
    led.init(Digital_IO::OUTPUT);

    UART_Comm uart_handle(usart0);

    static uint32_t led_timer;
    static uint32_t uart_timer;
    static uint32_t logger_timer;

    bool led_state = false;

    while(1) {
        uint32_t now = millis;

        if((led_timer < now) && (now - led_timer >= 500)) {
            led_state = !led_state;
            led_timer += 500;

            led.set_output(led_state);
        }
        
        if((logger_timer < now) && (now - logger_timer > 1000)) {
            uart_handle.puts("Hello world from AVR!");

            logger_timer += 1000;
        }

        if((uart_timer < now) && (now - uart_timer > 10)) {
            uart_handle.flush();

            uart_timer += 5;
        }
    }
}