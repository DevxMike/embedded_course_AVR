#ifndef gpio_h
#define gpio_h

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.hpp"
#include "interfaces.hpp"

#define MAX_PCINT_PINS 8
#define PCINT_BANKS_NUMBER 3

static constexpr uint8_t PCINT_DISABLED = 0xFF;

struct GPIO_t {
    volatile uint8_t* ddr_reg;
    volatile uint8_t* pin_reg;
    volatile uint8_t* port_reg;

    using pcint_callback_t = void(*)(GPIO_interface& io, void* context);

    volatile uint8_t* pcicr_reg;
    volatile uint8_t* pcifr_reg;
    volatile uint8_t* pcmsk_reg;

    uint8_t pcicr_bit;
};

struct PCINT_Bank {
    GPIO_interface* pins[MAX_PCINT_PINS];
};

extern PCINT_Bank pcint_banks[PCINT_BANKS_NUMBER];


class Digital_IO : public GPIO_interface {
    public:
        Digital_IO(GPIO_t& g, uint8_t p, uint8_t int_num = PCINT_DISABLED);
    
        void init(Direction d) override;
        void set_output(Output state) override;
        bool read_input() override;
        void on_pin_change() override;
    
        void attach_pcint(GPIO_t::pcint_callback_t cb, void* context = nullptr);
    
    private:
        volatile bool current;
        GPIO_t::pcint_callback_t pcint_callback;
        uint8_t pin_num;
        GPIO_t& regs;
        uint8_t pcint_num;
        void* additional_context;
    };

#endif