#include "../include/system_timer.hpp"

volatile uint32_t Timebase::millis = 0;
volatile uint32_t Timebase::microseconds = 0;

void Timebase::init(timer8_t& t) {
    t.compareA_cb = tick;

    *t.tccra |= (1 << WGM01);  
    *t.tccrb |= (1 << CS01);
    *t.ocra = 99; 
    *t.timsk |= (1 << OCIE0A); 
}

uint32_t Timebase::now() {
    return millis;
}

void Timebase::tick(timer8_t& t)
{
    (void)t;

    microseconds += 50;

    static uint8_t ms_div = 0;
    if (++ms_div >= 20) {
        ms_div = 0;
        ++millis;
    }
}