#ifndef system_timer_hpp
#define system_timer_hpp

#include "timer8_t.hpp"
#include "common_defs.hpp"

class Timebase {
public:
    static void init(timer8_t& t);
    static uint32_t now();
    
private:
    static void tick(timer8_t& t);

    static volatile uint32_t millis;
    static volatile uint32_t microseconds;
};

#endif