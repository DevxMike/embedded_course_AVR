#include "../include/system_timer.hpp"

volatile uint32_t Timebase::millis = 0;

void Timebase::init(timer8_t& t) {
    t.compareA_cb = tick;

    *t.tccra |= (1 << WGM01);  
    *t.tccrb |= (1 << CS01) | (1 << CS00); 
    *t.ocra = 249; 
    *t.timsk |= (1 << OCIE0A); 
}

uint32_t Timebase::now() {
    return millis;
}

void Timebase::tick(timer8_t& t) {
    ++millis;
}