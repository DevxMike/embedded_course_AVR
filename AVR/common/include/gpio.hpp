#ifndef gpio_h
#define gpio_h

#include <stdint.h>

struct GPIO_t {
    volatile uint8_t* ddr_reg;
    volatile uint8_t* pin_reg;
    volatile uint8_t* port_reg;
};

class GPIO_interface {
public:
    enum Direction : uint8_t {
        INPUT,
        INPUT_PULLUP,
        OUTPUT
    };

    enum Output : bool {
        LOW,
        HIGH
    };

    virtual void init(Direction d) = 0;
    virtual void set_output(Output state) = 0;
    virtual bool read_input() = 0;
};

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

    void set_output(Output state) override {
        if(state) {
            *regs.port_reg |= (1 << pin_num);
        }
        else {
            *regs.port_reg &= ~(1 << pin_num);
        }
    }

    bool read_input() override {
        return (*regs.pin_reg & (1 << pin_num));
    }

    Digital_IO(GPIO_t& g, uint8_t p) :
        regs{ g }, pin_num{ p } { }

private:
    GPIO_t& regs;
    uint8_t pin_num;
};

#endif