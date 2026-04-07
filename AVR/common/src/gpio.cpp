#include "../include/gpio.hpp"

// ========================
// ctor
// ========================

Digital_IO::Digital_IO(GPIO_t& g, uint8_t p, uint8_t int_num)
    : current{ false },
      pcint_callback{ nullptr },
      pin_num{ p },
      regs{ g },
      pcint_num{ int_num },
      additional_context{ nullptr }
{
    read_input();
}

// ========================
// init
// ========================

void Digital_IO::init(Direction d) {
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

// ========================
// output
// ========================

void Digital_IO::set_output(Output state) {
    if(state) {
        *regs.port_reg |= (1 << pin_num);
    }
    else {
        *regs.port_reg &= ~(1 << pin_num);
    }
}

// ========================
// input
// ========================

bool Digital_IO::read_input() {
    current = (*regs.pin_reg & (1 << pin_num));
    return current;
}

// ========================
// PCINT
// ========================

void Digital_IO::attach_pcint(GPIO_t::pcint_callback_t cb, void* context) {
    if (!regs.pcmsk_reg || !regs.pcicr_reg)
        return; 

    auto bank_idx = regs.pcicr_bit;

    if (bank_idx >= PCINT_BANKS_NUMBER) {
        return;
    }

    if (pcint_num == PCINT_DISABLED || pcint_num > 7)
        return;

    cli();

    *regs.pcmsk_reg |= (1 << pcint_num);
    *regs.pcicr_reg |= (1 << regs.pcicr_bit);
    
    pcint_callback = cb;
    additional_context = context;
    pcint_banks[bank_idx].pins[pcint_num] = this;

    read_input();

    sei();
}

// ========================
// ISR hook
// ========================

void Digital_IO::on_pin_change() {
    auto old_input = current;

    if (pcint_callback && old_input != read_input())
        pcint_callback(*this, additional_context);

    read_input();
}