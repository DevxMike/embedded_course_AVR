#ifndef led_driver_hpp
#define led_driver_hpp

// LCD commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

class LiquidCrystal : public Comm_IO {
public:
    // Constructors
    LiquidCrystal(GPIO_interface& rs, GPIO_interface& enable,
                  GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3,
                  GPIO_interface& d4, GPIO_interface& d5, GPIO_interface& d6, GPIO_interface& d7);

    LiquidCrystal(GPIO_interface& rs, GPIO_interface& rw, GPIO_interface& enable,
                  GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3,
                  GPIO_interface& d4, GPIO_interface& d5, GPIO_interface& d6, GPIO_interface& d7);

    LiquidCrystal(GPIO_interface& rs, GPIO_interface& enable,
                  GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3);

    LiquidCrystal(GPIO_interface& rs, GPIO_interface& rw, GPIO_interface& enable,
                  GPIO_interface& d0, GPIO_interface& d1, GPIO_interface& d2, GPIO_interface& d3);

    void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
    void clear();
    void home();
    void noDisplay();
    void display();
    void noBlink();
    void blink();
    void noCursor();
    void cursor();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();
    void setRowOffsets(int row1, int row2, int row3, int row4);
    void createChar(uint8_t location, uint8_t charmap[]);
    void setCursor(uint8_t col, uint8_t row);
    void command(uint8_t value);

    // Comm_IO interface
    virtual char putc(char c);
    virtual uint16_t puts(const char* s);
    virtual uint16_t put_buffer(const char* b, uint16_t len);
    virtual void flush();
    inline uint32_t write(uint8_t c) { return putc(static_cast<char>(c)); }

private:
    Custom::Optional<char> peek() const {
        return {};
    }

    Custom::Optional<char> getc() {
        return {};
    }

    void send(uint8_t value, bool mode);
    void write4bits(uint8_t value);
    void write8bits(uint8_t value);
    void pulseEnable();

    GPIO_interface* _rs_pin;
    GPIO_interface* _rw_pin;       // optional, can be nullptr
    GPIO_interface* _enable_pin;
    GPIO_interface* _data_pins[8];

    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    uint8_t _row_offsets[4];
};

#endif