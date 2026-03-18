#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include "../common/include/main.hpp"

#include <avr/io.h>
#include <util/delay.h>
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
Digital_IO lcd_data_pins[] = {
    {GPIOD_desc, PD4}, {GPIOD_desc, PD5}, {GPIOD_desc, PD6}, {GPIOD_desc, PD7}
};

GPIO_t GPIOC_desc = GPIOx_t(C);
Digital_IO lcd_ctrl_pins[] = {
    {GPIOC_desc, PC0}, // RS
    {GPIOC_desc, PC2}  // Enable
};

uint32_t display_refresh_counter { 0 };

LiquidCrystal lcd_4bit_no_rw(
    lcd_ctrl_pins[0], // RS
    lcd_ctrl_pins[1], // Enable
    lcd_data_pins[0], lcd_data_pins[1], lcd_data_pins[2], lcd_data_pins[3]
);

uint32_t adc_conversion_timer { 0 };

ADC_t adc0;

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;

    lcd_4bit_no_rw.begin(16, 2, LCD_5x8DOTS);

    auto& adc_inst = ADC_wrapper::instance();

    sei();

    const auto max = 1023.0f;
    const auto ref = 5.0f;

    uint16_t raw = 0;

    while(1) {
        if (Timebase::now() - adc_conversion_timer >= 100) {
            adc_inst.start_conversion();
            adc_conversion_timer = Timebase::now();
        }

        if (Timebase::now() - display_refresh_counter >= 100) {
            lcd_4bit_no_rw.clear();
            lcd_4bit_no_rw.setCursor(0, 0);

            raw = adc_inst.get_stored();

            auto voltage = uint16_t((raw / max) * ref * 1000.0f);

            char buf[17];
            sprintf(buf,"Value: %4d", raw);

            lcd_4bit_no_rw.puts(buf);

            sprintf(buf, "Voltage: %d,%0.3dV", 
                voltage / 1000, voltage % 1000);

            lcd_4bit_no_rw.setCursor(0, 1);
            lcd_4bit_no_rw.puts(buf);

            display_refresh_counter = Timebase::now();
        } 

        if (Timebase::now() - uart_timer >= 300) {
            uart_handle.flush();
            uart_timer = Timebase::now();
        }
    }
}