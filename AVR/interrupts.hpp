#ifndef interrupts_h
#define interrupts_h

#include <avr/interrupt.h>

#include "communication.hpp"
#include "timer8_t.hpp"
#include "main.hpp"

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
    if(timer2.timer_overflow_cb != nullptr) {
        timer2.timer_overflow_cb(timer0);
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