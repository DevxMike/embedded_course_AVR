#ifndef timer8_t_h
#define timer8_t_h

#include <stdint.h>

struct timer8_t {
    typedef void (*cback_type)(timer8_t&);

    volatile uint8_t* tccra;
    volatile uint8_t* tccrb;
    volatile uint8_t* tcnt;
    volatile uint8_t* ocra;
    volatile uint8_t* ocrb;
    volatile uint8_t* timsk;
    volatile uint8_t* tifr;

    cback_type compareB_cb;
    cback_type compareA_cb;
    cback_type timer_overflow_cb;
};

extern timer8_t timer0;
extern timer8_t timer2;

extern volatile uint32_t millis;

uint32_t get_timestamp() {
    return millis;
}

#endif