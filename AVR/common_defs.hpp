#ifndef common_defs_h
#define common_defs_h

#include "timer8_t.hpp"
#include "communication.hpp"
#include "gpio.hpp"

#define timer0_base ((timer8_t){ \
    .tccra = &TCCR0A, \
    .tccrb = &TCCR0B, \
    .tcnt = &TCNT0,   \
    .ocra = &OCR0A,   \
    .ocrb = &OCR0B,   \
    .timsk = &TIMSK0, \
    .tifr = &TIFR0,   \
    .compareB_cb = nullptr,   \
    .compareA_cb = nullptr,   \
    .timer_overflow_cb = nullptr \
    })

#define usart_base ((UART_t){   \
    .udr = &UDR0,      \
    .ucsra = &UCSR0A,  \
    .ucsrb = &UCSR0B,  \
    .ucsrc = &UCSR0C,  \
    .ubrrl = &UBRR0L,  \
    .ubrrh = &UBRR0H,  \
    .rx_complete_cback = nullptr,  \
    .tx_complete_cback = nullptr,  \
    .UDRIE_cback = nullptr,        \
    .flush_tx = nullptr,           \
    .busy = false                  \
    })

#define GPIOx_t(x) ((GPIO_t){ \
            .ddr_reg = &DDR##x, \
            .pin_reg = &PIN##x, \
            .port_reg = &PORT##x \
        })

#endif