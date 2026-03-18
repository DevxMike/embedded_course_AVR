#ifndef main_h
#define main_h

#define USE_UART   1
#define USE_TIMER0 1
#define USE_TIMER1 0
#define USE_TIMER2 0
#define USE_ADC    1
#define USE_TIMEBASE 1
#define USE_PCINT 0

#include "communication.hpp"
#include "lcd_driver.hpp"
#include "adc.hpp"
#include "system_timer.hpp"
#include "common_defs.hpp"
#include "system_timer.hpp"
#include "timer8_t.hpp"
#include "timer16_t.hpp"

#define DEBUG_PRINT_ENABLED 0

#endif