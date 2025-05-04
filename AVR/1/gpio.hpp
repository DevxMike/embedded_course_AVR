#ifndef gpio_h
#define gpio_h

#include <stdint.h>

struct GPIO_t {
    volatile uint8_t* ddr_reg;
    volatile uint8_t* pin_reg;
    volatile uint8_t* port_reg;
};

class Digital_IO {
public:
    enum Direction : uint8_t {
        INPUT,
        INPUT_PULLUP,
        OUTPUT
    };

    void init(Direction d) {
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

    void set_output(bool state) {
        if(state) {
            *regs.port_reg |= (1 << pin_num);
        }
        else {
            *regs.port_reg &= ~(1 << pin_num);
        }
    }

    bool read_input() {
        return (*regs.pin_reg & (1 << pin_num));
    }

    Digital_IO(GPIO_t& g, uint8_t p) :
        regs{ g }, pin_num{ p } { }

private:
    GPIO_t& regs;
    uint8_t pin_num;
};

#endif