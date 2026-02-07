#ifndef gpio_h
#define gpio_h

#include <stdint.h>
#include "interfaces.hpp"

#define MAX_PCINT_PINS 8
#define PCINT_BANKS_NUMBER 3

static constexpr uint8_t PCINT_DISABLED = 0xFF;

struct GPIO_t {
    volatile uint8_t* ddr_reg;
    volatile uint8_t* pin_reg;
    volatile uint8_t* port_reg;

    using pcint_callback_t = void(*)(GPIO_interface& io);

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
    void init(Direction d) override{
        switch(d) {
            case INPUT:
                *regs.ddr_reg &= ~(1 << pin_num);
                *regs.port_reg &= ~(1 << pin_num);
                break;
            
            case INPUT_PULLUP:
                *regs.ddr_reg &= ~(1 << pin_num);
                *regs.port_reg |= (1 << pin_num);   
                break;

            case OUTPUT:
                *regs.ddr_reg |= (1 << pin_num);
                break;
        }
    }

    void attach_pcint(GPIO_t::pcint_callback_t cb) {
        if (!regs.pcmsk_reg || !regs.pcicr_reg)
            return; 

        auto bank_idx = regs.pcicr_bit;

        if (bank_idx >= PCINT_BANKS_NUMBER) {
            return;
        }

        if (pcint_num == PCINT_DISABLED || pcint_num > 7)
            return;

        uint8_t sreg = SREG;

        cli();

        *regs.pcmsk_reg |= (1 << pcint_num);
        *regs.pcicr_reg |= (1 << regs.pcicr_bit);
        
        pcint_callback = cb;
        pcint_banks[bank_idx].pins[pcint_num] = this;

        read_input();

        SREG = sreg;
    }

    void set_output(Output state) override {
        if(state) {
            *regs.port_reg |= (1 << pin_num);
        }
        else {
            *regs.port_reg &= ~(1 << pin_num);
        }
    }

    bool read_input() override {
        current = (*regs.pin_reg & (1 << pin_num));
        return current;
    }

    void on_pin_change() override {
        auto old_input = current;

        if (pcint_callback && old_input != read_input())
            pcint_callback(*this);
        
        read_input();
    }

    Digital_IO(GPIO_t& g, uint8_t p, uint8_t int_num = PCINT_DISABLED) :
        regs{ g }, pin_num{ p }, pcint_num{ int_num }, pcint_callback{ nullptr } 
        {
            read_input();
        }

private:
    volatile bool current;
    GPIO_t& regs;
    uint8_t pin_num;
    uint8_t pcint_num;
    GPIO_t::pcint_callback_t pcint_callback;
};

#endif