#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include "../common/include/main.hpp"

#include "../common/include/timer8_t.hpp"
#include "../common/include/communication.hpp"
#include "../common/include/common_defs.hpp"
#include "../common/include/spi.hpp"
#include "../common/include/system_timer.hpp"

#include <avr/io.h>
#include <util/delay.h>

timer8_t timer0 = timer0_base;
     
static inline void init_hw_uart(UART_t& iface, uint16_t baudrate) {
    uint16_t ubrr = (F_CPU / (16UL * baudrate)) - 1;

    *iface.ubrrh = (ubrr >> 8) & 0xFF; 
    *iface.ubrrl = ubrr & 0xFF;

    *iface.ucsrc = (1 << UCSZ01) | (1 << UCSZ00); 
    *iface.ucsrb |= (1 << RXCIE0) | (1 << TXCIE0) | (1 << RXEN0) | (1 << TXEN0); 
}

void rx_callback(UART_t& iface) {
    iface.rx_buffer.push(*iface.udr);
}

void USART0_flush(UART_t& iface) {
    auto next = iface.tx_buffer.pop();

    if(next) {
        *iface.udr = next.get();
    } 
}

void tx_callback(UART_t& iface) {
    auto next = iface.tx_buffer.pop();
    
    if(next) {
        *iface.udr = next.get();
    }
    else {
        iface.busy = false;
    }
}

UART_t usart0 = usart_base;
Communication<UART_t> uart_handle(usart0);

GPIO_t gpio_d = GPIOx_t(D);
Digital_IO latch { gpio_d, PD2 };

GPIO_t gpio_b = GPIOx_t(B);
Digital_IO spi_pins[] {
    { gpio_b, PB3 },
    { gpio_b, PB5 }
};

Digital_IO ss_pin { gpio_b, PB2 };

void init_display_spi(SPI_wrapper &spi) {
    for (auto& pin : spi_pins) {
        pin.init(GPIO_interface::Direction::OUTPUT);
    }

    ss_pin.init(GPIO_interface::Direction::OUTPUT);

    spi.spi_enable(true);
    spi.spi_int_enable(true);
    spi.master_slave_select(true);
    spi.set_data_order(true);
    // spi.set_clock_polarity(false);
    // spi.set_clock_phase(false);
    spi.set_prescaler(SPI_wrapper::prescaler::kPresc3, false);

    latch.set_output(GPIO_interface::Output::HIGH);
    // spi.set_ss_pins(&ss_pin, 1);
}

void init_test(SPI_wrapper &spi, SPI_wrapper::prescaler p, bool spi2x) {
    for (auto& pin : spi_pins) {
        pin.init(GPIO_interface::Direction::OUTPUT);
    }
    ss_pin.init(GPIO_interface::Direction::OUTPUT);

    spi.spi_enable(true);
    spi.master_slave_select(true);
    spi.set_data_order(false);
    // spi.set_clock_polarity(false);
    // spi.set_clock_phase(false);
    spi.set_prescaler(p, spi2x);
    spi.spi_int_enable(true);
    // ss_pin.set_output(GPIO_interface::Output::HIGH);

    // latch.set_output(GPIO_interface::Output::HIGH);
    spi.set_ss_pins(&ss_pin, 1);
}

const char coder[] = {
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
}; // 0bxGFEDCBA format

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    // init_display_spi();


    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;

    uint8_t num { 0 };

    sei();

    auto& spi_inst = SPI_wrapper::instance();

    bool test_end { false };

    while (1) {
        
        if (!test_end && (Timebase::now() - delay_timer >= 500)) {
            // ++num;

            // if (num > 99) {
            //     num = 0;
            // }

            // auto dig1 = num % 10;
            // auto dig2 = num / 10;

            // char digits[2] {
            //     coder[dig2], coder[dig1]
            // };

            // for (const auto& d: digits) {
            //     spi_inst.putc(d);
            // }

            // latch.set_output(GPIO_interface::Output::LOW);
            // _delay_us(10);

            // spi_inst.flush();

            // _delay_us(10);
            // latch.set_output(GPIO_interface::Output::HIGH);

            // // spi_inst.put_buffer(digits, 2);

            // // spi_inst.flush();

            // // _delay_us(1000);
            // // latch.set_output(GPIO_interface::Output::LOW);
            // // _delay_us(1000);
            // // latch.set_output(GPIO_interface::Output::HIGH);

            auto base = static_cast<uint8_t>(SPI_wrapper::prescaler::kPresc1);
            auto num = static_cast<uint8_t>(SPI_wrapper::prescaler::kNumPrescalers);

            for (auto presc = base; presc < num; ++presc) {
                init_test(spi_inst, static_cast<SPI_wrapper::prescaler>(presc), false);
                spi_inst.putc(coder[0]);
                spi_inst.flush();
                _delay_ms(100);
            }

            for (auto presc = base; presc < num; ++presc) {
                init_test(spi_inst, static_cast<SPI_wrapper::prescaler>(presc), true);
                spi_inst.putc(coder[0]);
                spi_inst.flush();
                _delay_ms(100);
            }

            test_end = true;

            delay_timer = Timebase::now();
        }

        if (Timebase::now() - uart_timer >= 3000) {
            // uart_handle.puts("Hello\n\r");
            // uart_handle.flush();
            // uart_timer = Timebase::now();
        }
    }
}