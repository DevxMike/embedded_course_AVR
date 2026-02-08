#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "../common/include/main.hpp"
#include <string.h>
#include <stdio.h>


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

GPIO_t GPIOD_desc = GPIOx_t(D);
GPIO_t GPIOB_desc = GPIOx_t(B);
GPIO_t GPIOC_desc = GPIOx_t(C);

static uint8_t pcmsk_to_bank(volatile uint8_t* reg) {
    if (reg == &PCMSK0) return 0;
    if (reg == &PCMSK1) return 1;
    if (reg == &PCMSK2) return 2;
    return 0xFF;
}

void pcint_callback(GPIO_interface& pin) {
    char buf[50];

    auto& ppin = static_cast<Digital_IO&>(pin);
    
    sprintf(buf, "PCMSK [%u] | PCINT [%u] | input state: %u\n\r", 
        pcmsk_to_bank(ppin.regs.pcmsk_reg), 
        ppin.pcint_num, 
        pin.read_input()? 1 : 0
    );

    uart_handle.puts(buf);
}

Digital_IO pcint_pins[] = {
    // PORTB – PCINT0..5 (D8–D13)
    { (GPIOB_desc.pcmsk_reg = &PCMSK0, GPIOB_desc.pcicr_bit = 0, GPIOB_desc), PB0, PCINT0 },
    { GPIOB_desc, PB1, PCINT1 },
    { GPIOB_desc, PB2, PCINT2 },
    { GPIOB_desc, PB3, PCINT3 },
    { GPIOB_desc, PB4, PCINT4 },
    { GPIOB_desc, PB5, PCINT5 },

    // PORTC – PCINT11..13 (A3–A5)
    { (GPIOC_desc.pcmsk_reg = &PCMSK1, GPIOC_desc.pcicr_bit = 1, GPIOC_desc), PC3, PCINT11},
    { GPIOC_desc, PC4, PCINT12 },
    { GPIOC_desc, PC5, PCINT13 },

    // PORTD – PCINT18 (D2)
    { (GPIOD_desc.pcmsk_reg = &PCMSK2, GPIOD_desc.pcicr_bit = 2, GPIOD_desc), PD2, PCINT18 },
    { GPIOD_desc, PD3, PCINT19 }

};

void dump_digital_io(Digital_IO& pin) {
    char buf[200];

    sprintf(buf,
        "Digital_IO dump:\n\r"
        "  pin_num: %u\n\r"
        "  pcint_num: %u\n\r"
        "  current input: %u\n\r"
        "  regs.ddr_reg: %p\n\r"
        "  regs.port_reg: %p\n\r"
        "  regs.pin_reg: %p\n\r"
        "  regs.pcmsk_reg: %p\n\r"
        "  regs.pcicr_bit: %u\n\n\r",
        pin.pin_num,
        pin.pcint_num,
        pin.read_input()? 1 : 0,
        (void*)pin.regs.ddr_reg,
        (void*)pin.regs.port_reg,
        (void*)pin.regs.pin_reg,
        (void*)pin.regs.pcmsk_reg,
        pin.regs.pcicr_bit
    );

    uart_handle.puts(buf);
    uart_handle.flush();
}

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;
    // LiquidCrystal lcd_4bit_no_rw(
    //     lcd_ctrl_pins[0], // RS
    //     lcd_ctrl_pins[2], // Enable
    //     lcd_data_pins[4], lcd_data_pins[5], lcd_data_pins[6], lcd_data_pins[7]
    // );

    sei();



    for (auto& pin : pcint_pins) {
        pin.init(GPIO_interface::Direction::INPUT_PULLUP);
        pin.attach_pcint(pcint_callback);
    }
    
    for (uint8_t bank = 0; bank < PCINT_BANKS_NUMBER; ++bank) {
        for (uint8_t i = 0; i < MAX_PCINT_PINS; ++i) {
            if (pcint_banks[bank].pins[i]) {
                char buf[60];
                sprintf(buf, "Bank %u, PCINT index %u -> pin object at %p\n\n\r",
                        bank, i, (void*)pcint_banks[bank].pins[i]);
                uart_handle.puts(buf);
                uart_handle.flush();
                _delay_ms(50);
            }
        }
    }

    // for (auto& pin: pcint_pins) {
    //     dump_digital_io(pin);
    //     _delay_ms(50);
    // }


    while(1) {
        // test_lcd(lcd_4bit_no_rw, "4bit");
        
        if (Timebase::now() - uart_timer >= 200) {
            uart_handle.flush();
            uart_timer = Timebase::now();
        }
        // if (Timebase::now() - delay_timer >= 1000) {
        //     uart_handle.puts("Hello world!\n\r");
        //     delay_timer = Timebase::now();
        // }
    }
}