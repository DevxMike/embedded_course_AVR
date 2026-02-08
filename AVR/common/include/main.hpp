#ifndef main_h
#define main_h

#define USE_UART 1
#define USE_TIMER0 1
#define USE_TIMER2 0

#define USE_TIMEBASE 1

#include "system_timer.hpp"
#include "common_defs.hpp"
#include "communication.hpp"
#include "system_timer.hpp"
#include "timer8_t.hpp"
#include "lcd_driver.hpp"

#define USE_PCINT 1

#include "relative_encoder.hpp"

#define DEBUG_PRINT_ENABLED 1

#endif