#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "../common/include/main.hpp"
#include <stdio.h>
#include "menu.h"

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

GPIO_t GPIOB_desc = GPIOx_t(B);
Digital_IO RGB_pins[] = {
    { GPIOB_desc, PB0 }, { GPIOB_desc, PB1 }, { GPIOB_desc, PB2 } 
};

timer16_t timer1 = timer1_base;
timer8_t timer2 = timer2_base;

void init_hw_pwm_rgb() {
    // --- TIMER1 Red (OCR1A) & Green (OCR1B) ---
    // Fast PWM 8-bit, non-inverting
    *timer1.tccra = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
    *timer1.tccrb = (1 << CS10); // prescaler 1 → ~31 kHz PWM

    *timer1.ocral = 0; // Red
    *timer1.ocrbl = 0; // Green

    DDRB |= (1 << PB1) | (1 << PB2);

    *timer2.tccra = (1 << WGM20) | (1 << WGM21) | (1 << COM2B1); // Fast PWM, non-inverting OCR2B
    *timer2.tccrb = (1 << CS20); 

    *timer2.ocrb = 0; // Blue
    DDRD |= (1 << PD3); 
}

GPIO_t GPIOC_desc = GPIOx_t(C);
Digital_IO lcd_ctrl_pins[] = {
    {GPIOC_desc, PC0}, // RS
    {GPIOC_desc, PC2}  // Enable
};

Digital_IO enc_btn_pin = {
    GPIOC_desc, 
    PC3
};

GPIO_t GPIOC_enc_desc = GPIOx_PCINT_t(C, PCMSK1, 1);

Digital_IO enc_channels[] {
    { GPIOC_enc_desc, PC5, PCINT12 },
    { GPIOC_enc_desc, PD4, PCINT13}
};
uint32_t menu_refresh_counter { 0 };

LiquidCrystal lcd_4bit_no_rw(
    lcd_ctrl_pins[0], // RS
    lcd_ctrl_pins[1], // Enable
    lcd_data_pins[0], lcd_data_pins[1], lcd_data_pins[2], lcd_data_pins[3]
);

void on_button_press(GPIO_interface& btn) {
    Menu::button_handler(btn);
}

void encoder_change_callback(GPIO_interface& btn, void* context) {
    if (context != nullptr) {
        RotaryEncoder<uint8_t, 0, 255>* enc = (RotaryEncoder<uint8_t, 0, 255>*)context;
        enc->update();
    }
}

encoder_with_btn_setup_t enc_setup {
    {enc_channels[0], enc_channels[1], encoder_change_callback },
    { enc_btn_pin, Timebase::now, { on_button_press, nullptr } }
};

RotaryEncoderBtn<uint8_t, 0, 255> encoder { enc_setup };

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;

    lcd_4bit_no_rw.begin(16, 2, LCD_5x8DOTS);

    encoder.init();

    Menu::init(encoder, lcd_4bit_no_rw);
    Menu& instance = Menu::get();

    init_hw_pwm_rgb();

    sei();

    while(1) {
        if (Timebase::now() - menu_refresh_counter >= 50) {
            instance.poll();
            instance.render();
            menu_refresh_counter = Timebase::now();
        } 

        const auto& settings = instance.get_settings();

        *timer1.ocral = settings.red;   // Red
        *timer1.ocrbl = settings.green; // Green
        *timer2.ocrb  = settings.blue;  // Blue

        if (Timebase::now() - uart_timer >= 300) {
            uart_handle.flush();
            uart_timer = Timebase::now();
        }
    }
}