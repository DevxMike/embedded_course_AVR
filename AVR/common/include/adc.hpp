#ifndef adc_h
#define adc_h

#include <stdint.h>
#include <avr/io.h>
#include "main.hpp"

struct ADC_t {
    typedef void (*cback_type)(ADC_t&);

    volatile uint8_t* const admux  { &ADMUX  };
    volatile uint8_t* const adcsra { &ADCSRA };
    volatile uint8_t* const adcsrb { &ADCSRB };
    volatile uint8_t* const adcl   { &ADCL   };   
    volatile uint8_t* const adch   { &ADCH   };
    volatile uint8_t* const didr0  { &DIDR0  };

    cback_type on_conv_complete    { nullptr };
};

#if USE_ADC

extern ADC_t adc0;


class ADC_wrapper {
    public:
        enum class channel : uint8_t {
            kADC0 = 0, kADC1, kADC2, kADC3,
            kADC4, kADC5, kADC6, kADC7, kADC8,
            kNumChannels
        };
    
        enum class prescaler : uint8_t {
            kPRES0 = 0, kPRES1, kPRES2, kPRES3,
            kPRES4, kPRES5, kPRES6, kPRES7,
            kNumPres
        };
    
        enum class trigger_src : uint8_t {
            kFreeRun = 0, kAnalogComp, kEXTI0,
            kTCNT0CMPa, kTCNT0OVFL, kTCNT1CMPb,
            kTCNT1OVFL, kTCNT1CAP, kNumSrcs
        };
    
        enum class reference : uint8_t {
            kAREF = 0, kAVcc, kReserved, 
            kInternal, kNumRefs
        };
    
        static ADC_wrapper& instance();
    
        bool set_active_channel(channel c);
        void set_result_adjustment(bool a);
        void enable_adc(bool a);
        bool start_conversion();
        void enable_auto_trigger(bool a);
        void enable_interrupt(bool a);
        bool set_prescaler(prescaler p);
        bool set_trigger_src(trigger_src s);
        bool set_reference(reference r);
    
        uint16_t get_conv_direct(bool adj = false);
        volatile uint16_t get_stored() const;
    
    private:
        static void conversion_complete_cb(ADC_t& a);
    
        static uint8_t to_channel_idx(channel c);
        static uint8_t to_prescaler_idx(prescaler p);
        static uint8_t to_source_idx(trigger_src s);
        static uint8_t to_reference_idx(reference r);
    
    private:
        volatile uint16_t reading { 0 };
    
        ADC_wrapper() = default;
        ~ADC_wrapper() = default;
        ADC_wrapper& operator=(const ADC_wrapper&) = delete;
};
    

#endif


#endif