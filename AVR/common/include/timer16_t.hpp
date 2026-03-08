#ifndef timer16_t_h
#define timer16_t_h

#include <stdint.h>
#include "main.hpp"
#include <avr/interrupt.h>

struct timer16_t {
    typedef void (*cback_type)(timer16_t&);

    volatile uint8_t* tccra;
    volatile uint8_t* tccrb;
    volatile uint8_t* tccrc;

    volatile uint8_t* tcnth;
    volatile uint8_t* tcntl;
    volatile uint8_t* ocrah;
    volatile uint8_t* ocral;
    volatile uint8_t* ocrbh;
    volatile uint8_t* ocrbl;
    volatile uint8_t* icrh;
    volatile uint8_t* icrl;
    volatile uint8_t* timsk;
    volatile uint8_t* tifr;

    cback_type compareB_cb;
    cback_type compareA_cb;
    cback_type timer_overflow_cb;
    cback_type input_capture_cb;
};

#if USE_TIMER1

extern timer16_t timer1;

#endif

#endif