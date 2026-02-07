#ifndef interrupts_h
#define interrupts_h

#include <avr/interrupt.h>

#include "../include/communication.hpp"
#include "../include/timer8_t.hpp"
#include "../include/gpio.hpp"

PCINT_Bank pcint_banks[PCINT_BANKS_NUMBER] { nullptr };

#if USE_PCINT

ISR(PCINT0_vect) {
    uint8_t bank = 0;
    
    for (uint8_t i = 0; i < MAX_PCINT_PINS; ++i) {
        if (pcint_banks[bank].pins[i]) {
            pcint_banks[bank].pins[i]->on_pin_change();
        }
    }
}

ISR(PCINT1_vect) {
    uint8_t bank = 1;
    
    for (uint8_t i = 0; i < MAX_PCINT_PINS; ++i) {
        if (pcint_banks[bank].pins[i]) {
            pcint_banks[bank].pins[i]->on_pin_change();
        }
    }
}

ISR(PCINT2_vect) {
    uint8_t bank = 2;
    
    for (uint8_t i = 0; i < MAX_PCINT_PINS; ++i) {
        if (pcint_banks[bank].pins[i]) {
            pcint_banks[bank].pins[i]->on_pin_change();
        }
    }
}

#endif

#if USE_TIMER0

ISR(TIMER0_COMPA_vect) {
    if(timer0.compareA_cb != nullptr) {
        timer0.compareA_cb(timer0);
    }
}

ISR(TIMER0_COMPB_vect) {
    if(timer0.compareB_cb != nullptr) {
        timer0.compareB_cb(timer0);
    }
}

ISR(TIMER0_OVF_vect) {
    if(timer0.timer_overflow_cb != nullptr) {
        timer0.timer_overflow_cb(timer0);
    }
}

#endif

#if USE_TIMER2 

ISR(TIMER2_COMPA_vect) {
    if(timer2.compareA_cb != nullptr) {
        timer2.compareA_cb(timer0);
    }
}

ISR(TIMER2_COMPB_vect) {
    if(timer2.compareB_cb != nullptr) {
        timer2.compareB_cb(timer0);
    }
}

ISR(TIMER2_OVF_vect) {
    if(timer0.timer_overflow_cb != nullptr) {
        timer0.timer_overflow_cb(timer0);
    }
}

#endif

#if USE_UART

ISR(USART_TX_vect) {
    if(usart0.tx_complete_cback != nullptr) {
        usart0.tx_complete_cback(usart0);
    }
}

ISR(USART_RX_vect) {
    if(usart0.rx_complete_cback != nullptr) {
        usart0.rx_complete_cback(usart0);
    }
}

ISR(USART_UDRE_vect) {
    if(usart0.UDRIE_cback != nullptr) {
        usart0.UDRIE_cback(usart0);
    }
}

#endif

#endif