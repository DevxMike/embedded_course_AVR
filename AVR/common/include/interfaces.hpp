#ifndef interfaces_hpp
#define interfaces_hpp
#include <stdint.h>
#include "utils.hpp"
#include "timer8_t.hpp"


class GPIO_interface {
private:
    typedef void(*pcint_cb)(GPIO_interface& io, void* context);
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
    virtual void on_pin_change() = 0;
    virtual void attach_pcint(pcint_cb cb, void* context) = 0;
};

class Comm_IO {
public:
    ~Comm_IO() = default;

    virtual char putc(char c) = 0;
    virtual uint16_t puts(const char* s) = 0;
    virtual uint16_t put_buffer(const char* b, uint16_t len) = 0;
    virtual void flush() = 0;
    virtual Custom::Optional<char> peek() const = 0;
    virtual Custom::Optional<char> getc() = 0;
};

#endif

