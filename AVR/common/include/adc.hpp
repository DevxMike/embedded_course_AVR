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

#endif

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

    uint8_t to_channel_idx(channel c) {
        return static_cast<uint8_t>(c);
    }

    uint8_t to_prescaler_idx(prescaler p) {
        return static_cast<uint8_t>(p);
    }

    uint8_t to_source_idx(trigger_src s) {
        return static_cast<uint8_t>(s);
    }

    uint8_t to_reference_idx(reference r) {
        return static_cast<uint8_t>(r);
    }

    static ADC_wrapper& instance() {
        static ADC_wrapper inst;
        static bool initialized { false };

        if (!initialized) {
            adc0.on_conv_complete = conversion_complete_cb;
            inst.set_active_channel(channel::kADC5);
            inst.set_reference(reference::kAVcc);
            inst.enable_adc(true);
            inst.set_prescaler(prescaler::kPRES5);
            inst.enable_interrupt(true);
            
            initialized = true;
        }

        return inst;
    }

private:
    static void conversion_complete_cb(ADC_t& a) {
        auto& inst = ADC_wrapper::instance(); 
        inst.reading = inst.get_conv_direct();
    }

public:
    bool set_active_channel(channel c) {
        auto idx = to_channel_idx(c);
        const auto num = to_channel_idx(channel::kNumChannels);

        if (idx < num) {
            *adc0.admux = (*adc0.admux & 0xF0) | (idx & 0x0F);
            return true;
        }

        return false;
    }

    void set_result_adjustment(bool a) {
        if (a) {
            *adc0.admux |= (1 << ADLAR);
        }
        else {
            *adc0.admux &= ~(1 << ADLAR);
        }
    }

    void enable_adc(bool a) {
        if (a) {
            *adc0.adcsra |= (1 << ADEN);
        }
        else {
            *adc0.adcsra &= ~(1 << ADEN);
        }
    }

    bool start_conversion() {
        if (*adc0.adcsra & (1 << ADEN)) {
            *adc0.adcsra |= (1 << ADSC);
            return true;
        }

        return false;
    }

    void enable_auto_trigger(bool a) {
        if (a) {
            *adc0.adcsra |= (1 << ADATE);
        }
        else {
            *adc0.adcsra &= ~(1 << ADATE);
        }
    }

    void enable_interrupt(bool a) {
        if (a) {
            *adc0.adcsra |= (1 << ADIE);
        }
        else {
            *adc0.adcsra &= ~(1 << ADIE);
        }
    }

    bool set_prescaler(prescaler p) {
        auto idx = to_prescaler_idx(p);
        const auto num = to_prescaler_idx(prescaler::kNumPres);

        if (idx < num) {
            *adc0.adcsra = (*adc0.adcsra & ~(0x07)) | (idx & 0x07);

            return true;
        }   

        return false;
    }

    bool set_trigger_src(trigger_src s) {
        auto idx = to_source_idx(s);
        const auto num = to_source_idx(trigger_src::kNumSrcs);

        if (idx < num) {
            *adc0.adcsrb = (*adc0.adcsrb & ~0x07) | (idx & 0x07);

            return true;
        }

        return false;
    }

    bool set_reference(reference r) {
        auto idx = to_reference_idx(r);
        const auto num = to_reference_idx(reference::kNumRefs);
    
        if (idx < num && r != reference::kReserved) {
            constexpr uint8_t REFS_MASK = (1 << REFS0) | (1 << REFS1);
    
            *adc0.admux = (*adc0.admux & ~REFS_MASK) | (idx << REFS0);
    
            return true;
        }
    
        return false;
    }

    uint16_t get_conv_direct(bool adj = false) {
        uint16_t result = 0;

        if (!adj) {
            uint8_t low  = *adc0.adcl;
            uint8_t high = *adc0.adch;
            result = (high << 8) | low;
        }
        else {
            /// .... DIY
        }

        return result;
    }

    volatile uint16_t get_stored() const {
        return reading;
    }

private:
    ADC_t& handle { adc0 };
    volatile uint16_t reading { 0 };
};

#endif