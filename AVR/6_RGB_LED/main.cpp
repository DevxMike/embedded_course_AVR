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
Digital_IO lcd_data_pins[] = {
    {GPIOB_desc, PB0}, {GPIOB_desc, PB1}, {GPIOD_desc, PD2}, {GPIOD_desc, PD3},
    {GPIOD_desc, PD4}, {GPIOD_desc, PD5}, {GPIOD_desc, PD6}, {GPIOD_desc, PD7}
};

GPIO_t GPIOC_desc = GPIOx_t(C);
Digital_IO lcd_ctrl_pins[] = {
    {GPIOC_desc, PC0}, // RS
    {GPIOC_desc, PC1}, // RW
    {GPIOC_desc, PC2}  // Enable
};

Digital_IO enc_btn_pin = {
    GPIOC_desc, 
    PC3
};

GPIO_t GPIOD_enc_desc = GPIOx_t(D);

Digital_IO enc_channels[] {
    { (GPIOD_enc_desc.pcmsk_reg = &PCMSK2, GPIOD_enc_desc.pcicr_bit = 2, GPIOD_enc_desc), PD2, PCINT18 },
    { GPIOD_enc_desc, PD3, PCINT19}
};

void on_button_press(GPIO_interface& btn) {
    uart_handle.puts("Encoder button pressed!\n\r");
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

// void test_lcd(LiquidCrystal& lcd, const char* tc) {
//     lcd.begin(16, 2, LCD_5x8DOTS);
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.puts("Test RS/Enable");
//     lcd.setCursor(0, 1);
//     lcd.puts(tc);
//     lcd.cursor();
//     _delay_ms(500);
//     lcd.blink();
//     _delay_ms(500);
//     lcd.noCursor();
//     lcd.noBlink();
//     lcd.leftToRight();
//     lcd.rightToLeft();
//     lcd.autoscroll();
//     for(int i=0;i<16;i++) lcd.putc('A'+i);
//     lcd.noAutoscroll();
//     lcd.scrollDisplayLeft();
//     lcd.scrollDisplayRight();
//     lcd.home();
//     _delay_ms(3000);
// }

int main() {
    usart0.flush_tx = USART0_flush;
    usart0.rx_complete_cback = rx_callback;
    usart0.tx_complete_cback = tx_callback;

    init_hw_uart(usart0, 9600);

    Timebase::init(timer0);

    uint32_t uart_timer = 0;
    uint32_t delay_timer = 0;
    LiquidCrystal lcd_4bit_no_rw(
        lcd_ctrl_pins[0], // RS
        lcd_ctrl_pins[2], // Enable
        lcd_data_pins[4], lcd_data_pins[5], lcd_data_pins[6], lcd_data_pins[7]
    );

    enc_btn_pin.init(GPIO_interface::Direction::INPUT_PULLUP);
    encoder.init();

    sei();

    while(1) {
        // test_lcd(lcd_4bit_no_rw, "4bit");
        encoder.handleBtn();
        if (Timebase::now() - uart_timer >= 200) {
            char buf[50];
            sprintf(buf, "Encoder value: %ld\n\r", encoder.getValue());
            uart_handle.puts(buf);

            uart_handle.flush();
            uart_timer = Timebase::now();
        }

        // if (Timebase::now() - delay_timer >= 1000) {
        //     uart_handle.puts("Hello world!\n\r");
        //     delay_timer = Timebase::now();
        // }
    }
}