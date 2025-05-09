#ifndef leddisplay_hpp
#define leddisplay_hpp

#include "gpio.hpp"

#define DISPLAY_HAS_DOT 1
#define IS_COMMON_ANODE 1

#if IS_COMMON_ANODE
    #define SEGMENT_ON  false
    #define SEGMENT_OFF true
#else
    #define SEGMENT_ON  true
    #define SEGMENT_OFF false
#endif

const uint8_t seven_seg_digits[] = {
    0b00111111, // 0 → ABCDEF
    0b00000110, // 1 → BC
    0b01011011, // 2 → ABDEG
    0b01001111, // 3 → ABCDG
    0b01100110, // 4 → BCFG
    0b01101101, // 5 → ACDFG
    0b01111101, // 6 → ACDEFG
    0b00000111, // 7 → ABC
    0b01111111, // 8 → ABCDEFG
    0b01101111, // 9 → ABCDFG
    0b00000000  // inactive
};

constexpr uint8_t inactive_idx = 10;

struct SignleLEDSegment {
    uint8_t value;
#if DISPLAY_HAS_DOT
    bool dot_active;
#endif
};

template <uint8_t display_size>
class LEDDisplay {
private:
#if DISPLAY_HAS_DOT
    static constexpr uint8_t num_segment_pins = 8;
    enum {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G,
        DOT
    } SegmentMapping;
#else
    static constexpr uint8_t num_segment_pins = 7;
    enum {
        A = 0,
        B,
        C,
        D,
        E,
        F,
        G
    } SegmentMapping;
#endif

    SignleLEDSegment segments[display_size];
    Digital_IO* common_pins;
    Digital_IO* segment_pins;
    volatile uint8_t active_segment;

    void set_active(uint8_t com_active) {
        for(uint8_t i = 0; i < display_size; ++i) {
            common_pins[i].set_output(
                i == com_active? SEGMENT_ON : SEGMENT_OFF
            );
        }
    }

    void turn_off_coms() {
        set_active(display_size); 
    }

    void set_segment_pins(uint8_t display_num) {
        uint8_t digit = segments[display_num].value;
        uint8_t code = seven_seg_digits[digit <= inactive_idx? digit : inactive_idx];

#if DISPLAY_HAS_DOT
        for(uint8_t i = 0; i < num_segment_pins - 1; ++i){
#else
        for(uint8_t i = 0; i < num_segment_pins; ++i){
#endif
            segment_pins[i].set_output(                    
                (code & (1 << i))? SEGMENT_ON : SEGMENT_OFF
            );
        }
#if DISPLAY_HAS_DOT
        segment_pins[DOT].set_output(
            segments[display_num].dot_active? SEGMENT_ON : SEGMENT_OFF
        );
#endif
    }

public:

    LEDDisplay(Digital_IO* com, Digital_IO* seg)
        : common_pins(com), segment_pins(seg), active_segment(0) {}

    void next() {
        turn_off_coms();
        active_segment = (active_segment + 1) % display_size;
        set_segment_pins(active_segment);
        set_active(active_segment);
    }

    void set_segments(const SignleLEDSegment (&disp_vals)[display_size]) {
        for(uint8_t i = 0; i < display_size; ++i) {
            segments[i] = disp_vals[i];
        }
    }
};


#endif