#ifndef communication_h
#define communication_h

#include "utils.hpp"
#include "interfaces.hpp"
#include "cyclic_buffer.hpp"

#include "main.hpp"

const uint16_t buffer_size = 256;

struct UART_t {
    typedef void (*cback_type)(UART_t&);

    volatile uint8_t* udr;
    volatile uint8_t* ucsra;
    volatile uint8_t* ucsrb;
    volatile uint8_t* ucsrc;
    volatile uint8_t* ubrrl;
    volatile uint8_t* ubrrh;


    cback_type rx_complete_cback;
    cback_type tx_complete_cback;
    cback_type UDRIE_cback;
    cback_type flush_tx;

    bool busy;

    CyclicBuffer<char, buffer_size> rx_buffer;
    CyclicBuffer<char, buffer_size> tx_buffer;
};

#if USE_UART

extern UART_t usart0;

#endif

template <typename comm_iface_class>
class Communication : public Comm_IO {
public:
    Communication(comm_iface_class& i):
        iface { i } {}
    
    char putc(char c) final {
        iface.tx_buffer.push(c);
        return c;
    }

    uint16_t puts(const char* s) override {
        uint16_t num = 0;

        while(*s) {
            putc(*s);
            ++s; ++num;
        }

        return num;
    }

    uint16_t put_buffer(const char* b, uint16_t len) override {
        if(len > buffer_size) {
            return 0;
        }

        else {
            uint16_t i;

            for(i = 0; i < len; ++i) {
                putc(b[i]);
            }

            return i;
        }
    }

    void flush() override {
        if(iface.tx_buffer.empty() || iface.busy || iface.flush_tx == nullptr){
            return;
        }
        else {
            iface.flush_tx(iface);
            iface.busy = true;
        }
    }

    Custom::Optional<char> peek() const override {
        return iface.rx_buffer.peek();
    }

    Custom::Optional<char> getc() override {
        return iface.rx_buffer.pop();
    }

private:
    comm_iface_class& iface;
};

#endif